#include <SQLiteCpp/SQLiteCpp.h>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>

void create_tables(SQLite::Database &db) {
    db.exec(R"(
CREATE TABLE IF NOT EXISTS User (
    user_id INTEGER PRIMARY KEY NOT NULL,
    username TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS Chat (
    chat_id INTEGER PRIMARY KEY NOT NULL,
    title TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS UserChat (
    user_id INTEGER NOT NULL,
    chat_id INTEGER NOT NULL,
    FOREIGN KEY(user_id) REFERENCES User(user_id) ON DELETE RESTRICT,
    FOREIGN KEY(chat_id) REFERENCES Chat(chat_id) ON DELETE RESTRICT
);
CREATE TABLE IF NOT EXISTS Message (
    message_id INTEGER PRIMARY KEY NOT NULL,
    user_id INTEGER NOT NULL,
    chat_id INTEGER NOT NULL,
    body TEXT NOT NULL,
    sent_at_utc TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    FOREIGN KEY(user_id) REFERENCES User(user_id) ON DELETE RESTRICT,
    FOREIGN KEY(chat_id) REFERENCES Chat(chat_id) ON DELETE RESTRICT
)
    )");
}

using boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;

void process_client(tcp::socket s) {
    std::stringstream client_name;
    client_name << s.remote_endpoint() << " --> " << s.local_endpoint();

    auto log = [&]() -> std::ostream & {
        return std::cout << "[" << client_name.str() << "] ";
    };

    log() << "connected" << std::endl;
    beast::flat_buffer buffer;
    beast::error_code err;

    for (;;) {
        http::request<http::string_body> req;  // TODO: <void>?
        http::read(s, buffer, req, err);

        if (err == http::error::end_of_stream) {
            break;
        }
        if (err) {
            log() << "request read error: " << err << std::endl;
            break;
        }

        if (req.method() != http::verb::get) {
            // TODO: report "bad request" to the client
            log() << "bad request method, closing connection" << std::endl;
            break;
        }
        if (req.target() != "/chat") {
            // TODO: report "bad request" to the client
            // TODO: log injection
            log() << "unknown target '" << req.target()
                  << "', closing connection" << std::endl;
            break;
        }

        log() << "successful request" << std::endl;
        http::response<http::string_body> response(
            http::status::ok,
            req.version()
            );
        response.set(http::field::content_type, "text/html");
        response.keep_alive(req.keep_alive());
        static int counter = 1;
        response.body() = "Hello World to the chat! " + std::to_string(counter);
        counter++;
        response.prepare_payload();
        http::write(s, response, err);
        if (err) {
            log() << "write error" << std::endl;
            break;
        }
        if (response.need_eof()) {
            break;
        }
    }

    s.shutdown(tcp::socket::shutdown_send, err);
}

int main() {
    try {
        SQLite::Database db("server.sqlite3",
                            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        std::cout << "Opened database " << db.getFilename() << std::endl;

        db.exec("PRAGMA foreign_keys = ON");
        create_tables(db);

        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12397));
        std::cout << "Listening at " << acceptor.local_endpoint() << std::endl;

        for (;;) {
            std::thread(process_client, acceptor.accept()).detach();
        }
    } catch (std::exception &e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
