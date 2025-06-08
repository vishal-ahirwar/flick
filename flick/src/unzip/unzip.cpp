#include <filesystem>
#include <log/log.h>
#include <unzip/unzip.h>
#include <archive.h>
#include <archive_entry.h>
// Helper function to copy data blocks
int Unzip::copyData(struct archive* ar, struct archive* aw)
{
	int r;
	const void* buff;
	size_t size;
	la_int64_t offset; // Use la_int64_t for libarchive offsets

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF) {
			return ARCHIVE_OK;
		}
		if (r < ARCHIVE_OK) {
			return r; // Error during read
		}
		r = archive_write_data_block(aw, buff, size, offset);
		if (r < ARCHIVE_OK) {
			return r; // Error during write
		}
	}
}

bool Unzip::unzip(const std::string& archivePath, const std::string& outputDir)
{
	namespace fs = std::filesystem;

	struct archive* archiveReader = nullptr; // Initialize to nullptr
	struct archive* archiveWriter = nullptr; // Initialize to nullptr
	struct archive_entry* entry;
	int r;
	int flags;

	// Set extraction flags for security and preservation of file attributes
	flags = ARCHIVE_EXTRACT_TIME;		  // Preserve modification time
	flags |= ARCHIVE_EXTRACT_PERM;		  // Restore permissions
	flags |= ARCHIVE_EXTRACT_ACL;		  // Restore ACLs
	flags |= ARCHIVE_EXTRACT_FFLAGS;	  // Restore file flags (e.g., immutable)
	flags |= ARCHIVE_EXTRACT_XATTR;		  // Restore extended attributes
	flags |= ARCHIVE_EXTRACT_SECURE_NODOTDOT; // Crucial: Prevent path traversal (e.g., ../../../)
						  // flags |= ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS; // Crucial: Prevent writing to absolute paths
	flags |= ARCHIVE_EXTRACT_SECURE_NODOTDOT;

	// 1. Create archive reader object
	archiveReader = archive_read_new();
	if (!archiveReader) {
		Log::log("Error: Failed to allocate archive reader object.", Type::E_ERROR);
		return false;
	}

	// Enable support for all archive formats (ZIP, TAR, etc.) and compression filters (gzip, xz, bzip2, etc.)
	archive_read_support_format_all(archiveReader);
	archive_read_support_filter_all(archiveReader);

	// 2. Create archive writer object for writing to disk
	archiveWriter = archive_write_disk_new();
	if (!archiveWriter) {
		Log::log("Error: Failed to allocate archive writer object.", Type::E_ERROR);
		archive_read_free(archiveReader); // Clean up reader before exiting
		return false;
	}

	// Set extraction options for the writer
	archive_write_disk_set_options(archiveWriter, flags);
	// Set up standard lookup for user/group IDs and names
	archive_write_disk_set_standard_lookup(archiveWriter);

	// 3. Open the archive file for reading
	r = archive_read_open_filename(archiveReader, archivePath.c_str(), 10240); // 10KB block size
	if (r != ARCHIVE_OK) {
		Log::log(fmt::format("Error opening archive '{}': {}", archivePath, archive_error_string(archiveReader)), Type::E_ERROR);
		archive_read_free(archiveReader);
		archive_write_free(archiveWriter);
		return false;
	}

	Log::log(fmt::format("Extracting '{}' to '{}'", archivePath, outputDir));

	// 4. Loop through each entry in the archive
	while ((r = archive_read_next_header(archiveReader, &entry)) == ARCHIVE_OK) {
		const char* entryPathname = archive_entry_pathname(entry);
		fs::path fullOutputPath = fs::path(outputDir) / entryPathname;

		// Ensure the parent directory exists for the current entry
		fs::path parentPath = fullOutputPath.parent_path();
		if (!parentPath.empty() && !fs::exists(parentPath)) {
			try {
				fs::create_directories(parentPath);
			} catch (const fs::filesystem_error& e) {
				Log::log(fmt::format("Error creating directory '{}': {}", parentPath.string(), e.what()), Type::E_ERROR);
				// Don't just return false, try to continue with other files if possible,
				// or handle this more gracefully depending on desired behavior.
				// For now, we'll continue but log the error.
			}
		}

		// Set the entry's pathname for writing to the desired output directory
		archive_entry_set_pathname(entry, fullOutputPath.string().c_str());

		// Write the entry's header to disk (creates files/directories with correct permissions)
		r = archive_write_header(archiveWriter, entry);
		if (r != ARCHIVE_OK) {
			Log::log(fmt::format("Error finishing entry '{}' : {}", fullOutputPath.string(), archive_error_string(archiveWriter)),
				 Type::E_ERROR);
			// If header write fails, skip writing data for this entry
			archive_read_data_skip(archiveReader); // Important to skip data for failed entry
		} else {
			// Only copy data if it's a regular file or has a size
			if (archive_entry_size(entry) > 0 || archive_entry_filetype(entry) == AE_IFREG) {
				r = copyData(archiveReader, archiveWriter);
				if (r != ARCHIVE_OK) {
					Log::log(fmt::format("Error finishing entry '{}' : {}", fullOutputPath.string(),
							     archive_error_string(archiveWriter)),
						 Type::E_ERROR);
					// If data copy fails, we might still want to try finishing the entry
				}
			}
		}

		// Finish processing the current entry
		r = archive_write_finish_entry(archiveWriter);
		if (r != ARCHIVE_OK) {
			Log::log(fmt::format("Error finishing entry '{}' : {}", fullOutputPath.string(), archive_error_string(archiveWriter)),
				 Type::E_ERROR);
		}
	}

	if (r == ARCHIVE_FATAL) {
		Log::log(fmt::format("Fatal error encountered during archive reading: ", archive_error_string(archiveReader)), Type::E_ERROR);
		// Clean up before returning false for fatal error
		archive_read_close(archiveReader);
		archive_read_free(archiveReader);
		archive_write_close(archiveWriter);
		archive_write_free(archiveWriter);
		return false;
	} else if (r < ARCHIVE_OK && r != ARCHIVE_EOF) {
		// Non-fatal error or warning
		Log::log(fmt::format("Warning during archive processing: {}", archive_error_string(archiveReader)), Type::E_WARNING);
	}

	// 5. Clean up
	archive_read_close(archiveReader);
	archive_read_free(archiveReader);
	archive_write_close(archiveWriter);
	archive_write_free(archiveWriter);

	Log::log("Archive extraction complete.\n");
	return true;
}