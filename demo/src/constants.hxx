#ifndef _CONSTANTS_HXX_
#define _CONSTANTS_HXX_
#include <string_view>
constexpr std::string_view HOME_PAGE{R"(
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
        )"};
#endif