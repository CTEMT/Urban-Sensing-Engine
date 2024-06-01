#include "uspe.hpp"

int main(int argc, char const *argv[])
{
    uspe::uspe service;
    service.network::server::start();
    return 0;
}
