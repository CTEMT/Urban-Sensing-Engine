#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>

int main(int argc, char const *argv[])
{
    std::string root = USE_ROOT;
    std::string mongodb_host = USE_MONGODB_HOST;
    std::string mongodb_port = USE_MONGODB_PORT;

    mongocxx::instance inst{}; // This should be done only once.
    mongocxx::client conn{mongocxx::uri{"mongodb://" + mongodb_host + ":" + mongodb_port}};
    mongocxx::v_noabi::database db = conn[root];
#ifdef DROP_DATABASE
    db.drop();
#endif

    return 0;
}
