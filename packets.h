

#ifndef PACKETS
#define PACKETS

struct master_pkt {
    sc_uint<4>  awid;
    sc_uint<32> awadr;
    sc_uint<4>  awlen;
    sc_uint<3>  awsize;
    sc_uint<2>  awburst;
    sc_uint<2>  awlock;
    sc_uint<4>  awcache;
    sc_uint<3>  awprot;
    sc_uint<4> wid;
    sc_uint<32> wrdata;
    sc_uint<4>  wstrb;
    sc_uint<4> bid;
    sc_uint<4> arid;
    sc_uint<32> araddr;
    sc_uint<4> arlen;
    sc_uint<2> arburst;
    sc_uint<3>  arsize;
    sc_uint<2>  arlock;
    sc_uint<4>  arcache;
    sc_uint<3>  arprot;
    sc_uint<1> arvalid;
    sc_uint<1> rready;
    sc_uint<1> bready;
    sc_uint<1>        wlast;
    sc_uint<1>      awvalid;
    sc_uint<1>        wvalid;
};

struct slave_pkt {
    sc_uint<1> awready;
    sc_uint<1>  wready;
    sc_uint<4> bid;
    sc_uint<2> bresp;
    sc_uint<1> bvalid;
    sc_uint<1> arready;
    sc_uint<4> rid;
    sc_uint<32> rdata;
    sc_uint<2> rresp;
    sc_uint<1> rlast;
    sc_uint<1> rvalid;
};
#endif
