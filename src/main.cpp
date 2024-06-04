#include "uspe_db.hpp"
#include "uspe.hpp"

int main(int argc, char const *argv[])
{
    mongocxx::instance inst{}; // This should be done only once.

    uspe::uspe service{std::make_unique<uspe::uspe_db>()};
    service.network::server::start();
    return 0;
}
