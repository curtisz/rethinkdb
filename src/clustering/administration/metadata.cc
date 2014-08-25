// Copyright 2010-2014 RethinkDB, all rights reserved.
#include "clustering/administration/metadata.hpp"

#include "clustering/administration/database_metadata.hpp"
#include "clustering/administration/servers/machine_metadata.hpp"
#include "containers/archive/archive.hpp"
#include "containers/archive/boost_types.hpp"
#include "containers/archive/cow_ptr_type.hpp"
#include "containers/archive/stl_types.hpp"
#include "containers/archive/versioned.hpp"
#include "rdb_protocol/protocol.hpp"
#include "region/region_map_json_adapter.hpp"
#include "stl_utils.hpp"

/* RSI(reql_admin): The ReQL admin API changes involve big changes to the semilattice
metadata. Because many of the underlying concepts have changed, it's not clear if/how to
migrate serialized metadata from pre-ReQL-admin versions into the new format. GitHub
issue #2869 is tracking this discussion. As a temporary measure so that development can
proceed, deserialization of old versions has been disabled. */

RDB_IMPL_SERIALIZABLE_1_SINCE_v1_13(database_semilattice_metadata_t, name);
RDB_IMPL_SEMILATTICE_JOINABLE_1(database_semilattice_metadata_t, name);
RDB_IMPL_EQUALITY_COMPARABLE_1(database_semilattice_metadata_t, name);

RDB_IMPL_SERIALIZABLE_1_SINCE_v1_13(databases_semilattice_metadata_t, databases);
RDB_IMPL_SEMILATTICE_JOINABLE_1(databases_semilattice_metadata_t, databases);
RDB_IMPL_EQUALITY_COMPARABLE_1(databases_semilattice_metadata_t, databases);

RDB_IMPL_SERIALIZABLE_2(machine_semilattice_metadata_t, name, tags);
template void serialize<cluster_version_t::v1_15_is_latest>(
            write_message_t *, const machine_semilattice_metadata_t &);
template archive_result_t deserialize<cluster_version_t::v1_15_is_latest>(
            read_stream_t *, machine_semilattice_metadata_t *);
RDB_IMPL_SEMILATTICE_JOINABLE_2(machine_semilattice_metadata_t, name, tags);
RDB_IMPL_EQUALITY_COMPARABLE_2(machine_semilattice_metadata_t, name, tags);

RDB_IMPL_SERIALIZABLE_1(machines_semilattice_metadata_t, machines);
template void serialize<cluster_version_t::v1_15_is_latest>(
            write_message_t *, const machines_semilattice_metadata_t &);
template archive_result_t deserialize<cluster_version_t::v1_15_is_latest>(
            read_stream_t *, machines_semilattice_metadata_t *);
RDB_IMPL_SEMILATTICE_JOINABLE_1(machines_semilattice_metadata_t, machines);
RDB_IMPL_EQUALITY_COMPARABLE_1(machines_semilattice_metadata_t, machines);

RDB_IMPL_ME_SERIALIZABLE_2_SINCE_v1_13(ack_expectation_t, expectation_, hard_durability_);

RDB_IMPL_SERIALIZABLE_3(table_config_t::shard_t,
                        split_point, replica_names, director_names);
template void serialize<cluster_version_t::v1_15_is_latest>(
            write_message_t *, const table_config_t::shard_t &);
template archive_result_t deserialize<cluster_version_t::v1_15_is_latest>(
            read_stream_t *, table_config_t::shard_t *);
RDB_IMPL_EQUALITY_COMPARABLE_3(table_config_t::shard_t,
                               split_point, replica_names, director_names);

RDB_IMPL_SERIALIZABLE_1(table_config_t, shards);
template void serialize<cluster_version_t::v1_15_is_latest>(
            write_message_t *, const table_config_t &);
template archive_result_t deserialize<cluster_version_t::v1_15_is_latest>(
            read_stream_t *, table_config_t *);
RDB_IMPL_EQUALITY_COMPARABLE_1(table_config_t, shards);

RDB_IMPL_SERIALIZABLE_2(table_replication_info_t, config, chosen_directors);
template void serialize<cluster_version_t::v1_15_is_latest>(
            write_message_t *, const table_replication_info_t &);
template archive_result_t deserialize<cluster_version_t::v1_15_is_latest>(
            read_stream_t *, table_replication_info_t *);
RDB_IMPL_EQUALITY_COMPARABLE_2(table_replication_info_t, config, chosen_directors);

RDB_IMPL_SERIALIZABLE_4(namespace_semilattice_metadata_t,
                        name, database, primary_key, replication_info);
template void serialize<cluster_version_t::v1_15_is_latest>(
            write_message_t *, const namespace_semilattice_metadata_t &);
template archive_result_t deserialize<cluster_version_t::v1_15_is_latest>(
            read_stream_t *, namespace_semilattice_metadata_t *);

RDB_IMPL_SEMILATTICE_JOINABLE_4(
        namespace_semilattice_metadata_t,
        name, database, primary_key, replication_info);
RDB_IMPL_EQUALITY_COMPARABLE_4(
        namespace_semilattice_metadata_t,
        name, database, primary_key, replication_info);

RDB_IMPL_SERIALIZABLE_1(namespaces_semilattice_metadata_t, namespaces);
template void serialize<cluster_version_t::v1_15_is_latest>(
            write_message_t *, const namespaces_semilattice_metadata_t &);
template archive_result_t deserialize<cluster_version_t::v1_15_is_latest>(
            read_stream_t *, namespaces_semilattice_metadata_t *);
RDB_IMPL_SEMILATTICE_JOINABLE_1(namespaces_semilattice_metadata_t, namespaces);
RDB_IMPL_EQUALITY_COMPARABLE_1(namespaces_semilattice_metadata_t, namespaces);

RDB_IMPL_SERIALIZABLE_1_SINCE_v1_13(namespaces_directory_metadata_t, reactor_bcards);
RDB_IMPL_EQUALITY_COMPARABLE_1(namespaces_directory_metadata_t, reactor_bcards);

RDB_IMPL_SERIALIZABLE_3(
        cluster_semilattice_metadata_t,
        rdb_namespaces, machines, databases);
template void serialize<cluster_version_t::v1_15_is_latest>(
            write_message_t *, const cluster_semilattice_metadata_t &);
template archive_result_t deserialize<cluster_version_t::v1_15_is_latest>(
            read_stream_t *, cluster_semilattice_metadata_t *);
RDB_IMPL_SEMILATTICE_JOINABLE_3(cluster_semilattice_metadata_t,
                                rdb_namespaces, machines, databases);
RDB_IMPL_EQUALITY_COMPARABLE_3(cluster_semilattice_metadata_t,
                               rdb_namespaces, machines, databases);

RDB_IMPL_SERIALIZABLE_1_SINCE_v1_13(auth_semilattice_metadata_t, auth_key);
RDB_IMPL_SEMILATTICE_JOINABLE_1(auth_semilattice_metadata_t, auth_key);
RDB_IMPL_EQUALITY_COMPARABLE_1(auth_semilattice_metadata_t, auth_key);

RDB_IMPL_SERIALIZABLE_11(cluster_directory_metadata_t,
                         rdb_namespaces, machine_id, peer_id, cache_size, ips,
                         get_stats_mailbox_address, get_outdated_indexes_mailbox,
                         log_mailbox, server_name_business_card, local_issues,
                         peer_type);
INSTANTIATE_SERIALIZABLE_FOR_CLUSTER(cluster_directory_metadata_t);

bool ack_expectation_t::operator==(ack_expectation_t other) const {
    return expectation_ == other.expectation_ && hard_durability_ == other.hard_durability_;
}

void debug_print(printf_buffer_t *buf, const ack_expectation_t &x) {
    buf->appendf("ack_expectation{durability=%s, acks=%" PRIu32 "}",
                 x.is_hardly_durable() ? "hard" : "soft", x.expectation());
}

// json adapter concept for ack_expectation_t
json_adapter_if_t::json_adapter_map_t get_json_subfields(ack_expectation_t *target) {
    json_adapter_if_t::json_adapter_map_t res;
    res["expectation"] = boost::shared_ptr<json_adapter_if_t>(new json_adapter_t<uint32_t>(&target->expectation_));
    res["hard_durability"] = boost::shared_ptr<json_adapter_if_t>(new json_adapter_t<bool>(&target->hard_durability_));
    return res;
}
cJSON *render_as_json(ack_expectation_t *target) {
    return render_as_directory(target);
}

void apply_json_to(cJSON *change, ack_expectation_t *target) {
    apply_as_directory(change, target);
}


// ctx-less json adapter concept for cluster_directory_metadata_t
json_adapter_if_t::json_adapter_map_t get_json_subfields(cluster_directory_metadata_t *target) {
    json_adapter_if_t::json_adapter_map_t res;
    res["rdb_namespaces"] = boost::shared_ptr<json_adapter_if_t>(new json_adapter_t<namespaces_directory_metadata_t>(&target->rdb_namespaces));
    res["machine_id"] = boost::shared_ptr<json_adapter_if_t>(new json_adapter_t<machine_id_t>(&target->machine_id)); // TODO: should be 'me'?
    res["peer_id"] = boost::shared_ptr<json_adapter_if_t>(new json_adapter_t<peer_id_t>(&target->peer_id));
    res["cache_size"] = boost::shared_ptr<json_adapter_if_t>(new json_adapter_t<uint64_t>(&target->cache_size));
    res["ips"] = boost::shared_ptr<json_adapter_if_t>(new json_adapter_t<std::vector<std::string> >(&target->ips));
    res["peer_type"] = boost::shared_ptr<json_adapter_if_t>(new json_adapter_t<cluster_directory_peer_type_t>(&target->peer_type));
    return res;
}

cJSON *render_as_json(cluster_directory_metadata_t *target) {
    return render_as_directory(target);
}

void apply_json_to(cJSON *change, cluster_directory_metadata_t *target) {
    apply_as_directory(change, target);
}




// ctx-less json adapter for cluster_directory_peer_type_t
json_adapter_if_t::json_adapter_map_t get_json_subfields(cluster_directory_peer_type_t *) {
    return std::map<std::string, boost::shared_ptr<json_adapter_if_t> >();
}

cJSON *render_as_json(cluster_directory_peer_type_t *peer_type) {
    switch (*peer_type) {
    case SERVER_PEER:
        return cJSON_CreateString("server");
    case PROXY_PEER:
        return cJSON_CreateString("proxy");
    default:
        break;
    }
    return cJSON_CreateString("unknown");
}

void apply_json_to(cJSON *, cluster_directory_peer_type_t *) { }

// ctx-less json adapter concept for namespaces_directory_metadata_t
json_adapter_if_t::json_adapter_map_t get_json_subfields(namespaces_directory_metadata_t *target) {
    json_adapter_if_t::json_adapter_map_t res;
    res["reactor_bcards"] = boost::shared_ptr<json_adapter_if_t>(new json_read_only_adapter_t<std::map<namespace_id_t, directory_echo_wrapper_t<cow_ptr_t<reactor_business_card_t> > > >(&target->reactor_bcards));
    return res;
}

cJSON *render_as_json(namespaces_directory_metadata_t *target) {
    return render_as_directory(target);
}

void apply_json_to(cJSON *change, namespaces_directory_metadata_t *target) {
    apply_as_directory(change, target);
}

