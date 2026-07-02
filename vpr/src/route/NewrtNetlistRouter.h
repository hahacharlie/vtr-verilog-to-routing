#pragma once

/** @file NEWRT case for \ref NetlistRouter: evolved netlist router seeded from
 * the serial netlist router (loop through nets, one NewrtConnectionRouter). */

#include "netlist_routers.h"
#include "newrt_connection_router.h"

template<typename HeapType>
class NewrtNetlistRouter : public NetlistRouter {
  public:
    NewrtNetlistRouter(
        const Netlist<>& net_list,
        const RouterLookahead* router_lookahead,
        const t_router_opts& router_opts,
        CBRR& connections_inf,
        NetPinsMatrix<float>& net_delay,
        const ClusteredPinAtomPinsLookup& netlist_pin_lookup,
        std::shared_ptr<SetupHoldTimingInfo> timing_info,
        NetPinTimingInvalidator* pin_timing_invalidator,
        route_budgets& budgeting_inf,
        const RoutingPredictor& routing_predictor,
        const vtr::vector<ParentNetId, std::vector<std::unordered_map<RRNodeId, int>>>& choking_spots,
        bool is_flat,
        int route_verbosity)
        : _router(_make_router(router_lookahead, router_opts, is_flat, route_verbosity))
        , _net_list(net_list)
        , _router_opts(router_opts)
        , _connections_inf(connections_inf)
        , _net_delay(net_delay)
        , _netlist_pin_lookup(netlist_pin_lookup)
        , _timing_info(timing_info)
        , _pin_timing_invalidator(pin_timing_invalidator)
        , _budgeting_inf(budgeting_inf)
        , _routing_predictor(routing_predictor)
        , _choking_spots(choking_spots)
        , _is_flat(is_flat)
        , _route_verbosity(route_verbosity) {}
    ~NewrtNetlistRouter() {}

    RouteIterResults route_netlist(int itry, float pres_fac, float worst_neg_slack);
    void handle_bb_updated_nets(const std::vector<ParentNetId>& nets);
    void set_rcv_enabled(bool x);
    void set_timing_info(std::shared_ptr<SetupHoldTimingInfo> timing_info);

  private:
    std::unique_ptr<ConnectionRouterInterface> _make_router(const RouterLookahead* router_lookahead,
                                                            const t_router_opts& router_opts,
                                                            bool is_flat,
                                                            int route_verbosity) {
        auto& device_ctx = g_vpr_ctx.device();
        auto& route_ctx = g_vpr_ctx.mutable_routing();

        (void)router_opts;
        // NEWRT Connection Router (seeded from the serial connection router)
        return std::make_unique<NewrtConnectionRouter<HeapType>>(
            device_ctx.grid,
            *router_lookahead,
            device_ctx.rr_graph.rr_nodes(),
            &device_ctx.rr_graph,
            device_ctx.rr_rc_data,
            device_ctx.rr_graph.rr_switch(),
            route_ctx.rr_node_route_inf,
            is_flat,
            route_verbosity);
    }
    /* Context fields */
    std::unique_ptr<ConnectionRouterInterface> _router;
    const Netlist<>& _net_list;
    const t_router_opts& _router_opts;
    CBRR& _connections_inf;
    NetPinsMatrix<float>& _net_delay;
    const ClusteredPinAtomPinsLookup& _netlist_pin_lookup;
    std::shared_ptr<SetupHoldTimingInfo> _timing_info;
    NetPinTimingInvalidator* _pin_timing_invalidator;
    route_budgets& _budgeting_inf;
    const RoutingPredictor& _routing_predictor;
    const vtr::vector<ParentNetId, std::vector<std::unordered_map<RRNodeId, int>>>& _choking_spots;
    bool _is_flat;
    int _route_verbosity;
};

#include "NewrtNetlistRouter.tpp"
