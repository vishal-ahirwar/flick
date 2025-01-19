// Auto Generated C++ file by aura CLI
// Copyright 2023 Vishal Ahirwar
#include <cstdio>       // For fprintf
#include <democonfig.h> // Custom configuration (assumed to be for demo project)
#include <fmt/core.h>   // For formatted printing
#include <fmt/color.h>  // For colored text output
#include <crow.h>       // For basic web server
#include <asio.hpp>     // For asynchronous I/O
#include <constants.hxx>

int main(int argc, char *argv[])
{
    // Using fmt/core.h and fmt/color.h
    fmt::print(fmt::fg(fmt::color::crimson) | fmt::emphasis::bold, "Project Name: {}, {}\nVersion : {}.{}.{}\n", "demo", DEMO_COPYRIGHT, DEMO_VERSION_MAJOR, DEMO_VERSION_MINOR, DEMO_VERSION_PATCH);

    // Setting up a simple Crow server
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")
    ([]()
     { return R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Simple Crow Page</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    background-color: #f4f4f9;
                    color: #333;
                    margin: 0;
                    padding: 0;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                }
                .container {
                    text-align: center;
                    padding: 20px;
                    border: 1px solid #ddd;
                    border-radius: 10px;
                    background-color: #fff;
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                }
                h1 {
                    color: #5a5a5a;
                }
                p {
                    font-size: 1.2em;
                }
                a {
                    text-decoration: none;
                    color: #007bff;
                }
                a:hover {
                    text-decoration: underline;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>Welcome to Crow!</h1>
                <p>This is a simple HTML page served using Crow.</p>
                <p><a href="https://github.com/CrowCpp/Crow">Learn more about Crow</a></p>
            </div>
        </body>
        </html>
        )"; });
    // Running the server in a separate thread using asio
    asio::io_context io_context;
    std::thread crow_thread([&app]()
                            { app.port(8080).multithreaded().run(); });

    // Using asio for a simple timer
    asio::steady_timer timer(io_context, asio::chrono::seconds(3));
    timer.async_wait([](const asio::error_code &)
                     { fmt::print(fmt::fg(fmt::color::green) | fmt::emphasis::bold, "Timer expired!\n"); });

    // Run the asio context to process the timer event
    io_context.run();

    // Join the Crow server thread before exiting
    crow_thread.join();

    return 0;
}
