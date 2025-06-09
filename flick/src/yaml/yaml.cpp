#include <yaml-cpp/yaml.h>
#include <yaml/yaml.h>
bool Yaml::init()
{
	// read yaml
	// if yaml is not present at ~/flick/template/code.yaml
	// download the code.yaml from flick repo
	// read and fill the TemplateCode
	// return success
	// if all these failed return failed
}
const TemplateCode& Yaml::getCode() const { return mCode; }