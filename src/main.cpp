#include "uspe_db.hpp"
#include "uspe_server.hpp"

int main(int argc, char const *argv[])
{
    mongocxx::instance inst{}; // This should be done only once.

    uspe::uspe_server server{std::make_unique<uspe::uspe_db>()};
    server.start();
    return 0;
}
