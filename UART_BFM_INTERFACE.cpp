/*
File Name: UART_BFM_SLAVE.cpp
Description: AXI4 generic slave Module.
Created by :  Yara Disi
Creation Date: 2023
Contributors:
Last Modified Date: 8/26/2023
Version: 1.1
Copyright (c) Orion VLSI Technologies, 2023
This unpublished material is proprietary to Orion VLSI Technologies.
All Rights Reserved Worldwide.
Anything described herein is considered business secrets and confidential.
Reproduction or distribution, in whole or in part, is
forbidden except by express written permission of Orion VLSI Technologies.
 */

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc.h>
#include <iostream>
#include <iomanip>
#include "axi_defines.h"
#include "packets.h"
#include "UART_CONTROLLER.cpp"

using namespace sc_dt;
using namespace std;
using namespace sc_core;

#include <tlm.h>
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

SC_MODULE(UART_BFM_SLAVE) {

    sc_in<bool>      aclk;
    sc_in<bool>    aresetn;   //Active LOW
    sc_in<sc_dt::uint64> SLAVE_ID;

    master_pkt master_pktt;
    tlm_utils::simple_target_socket<UART_BFM_SLAVE> masterpkt_socket;

    slave_pkt slave_pktt;
    tlm_utils::simple_initiator_socket<UART_BFM_SLAVE> slavepkt_socket; //initiator = out

    // Internal Signals
     // Define signals and modules here
    
    sc_signal<bool> tx_start;
    sc_signal<sc_uint<8>> transmitting_data;
    sc_signal<sc_uint<1>> tx;
    sc_signal<bool> tx_busy;
    sc_signal<sc_uint<1>> rx;
    sc_signal<bool> rx_busy;
    sc_signal<sc_uint<8>> received_data;

    // Instantiate your UART_CONTROLLER module
    UART_CONTROLLER<8, 9600, 100> uart_controller;

    

    int i;

    // Write transaction internal buffers
    sc_uint<32> write_address;
    sc_uint<32> write_start_address;
    sc_uint<8> write_length;
    sc_uint<2> write_type;
    sc_uint<32> extended_strobe;
    sc_uint<4> strobe;
    sc_uint<3> write_size; // Only single word(4bytes)/transfer supported
    sc_uint<32> write_wrap_boundary = 0x000F;

    // Read transaction internal buffers
    sc_uint<32> read_address;
    sc_uint<32> read_start_address;
    sc_uint<8> read_length;
    sc_uint<2> read_type;
    sc_uint<3> read_size; // Only single word(4bytes)/transfer supported
    sc_uint<32> read_wrap_boundary = 0x000F;


    ///////////////////////////// Thread enable signals //////////////////////////////
    bool slave_aw_handshaking_enable = 0;
    bool slave_wresponse_handshaking_enable = 0;
    bool slave_write_channel_enable = 0;
    bool slave_rresponse_handshaking_enable = 0;
    bool slave_ar_handshaking_enable = 0;
    bool slave_read_channel_enable = 0;

    void init_thread() {
        slave_pktt.awready = 1;
        slave_pktt.wready = 0;
        slave_pktt.arready = 1;
        slave_pktt.rdata = 0;
        slave_pktt.bid = 0;
        slave_pktt.bresp = 0;
        slave_pktt.bvalid = 0;
        slave_pktt.rid = 0;
        slave_pktt.rresp = 0;
        slave_pktt.rlast = 0;
        slave_pktt.rvalid = 0;

        
        slave_aw_handshaking_enable = 0;
        slave_wresponse_handshaking_enable = 0;
        slave_write_channel_enable = 0;
        slave_rresponse_handshaking_enable = 0;
        slave_ar_handshaking_enable = 0;
        slave_read_channel_enable = 0;
        cout << "SLAVE" << SLAVE_ID << ": slave initialized! ID: " << SLAVE_ID.read() << std::endl;

    }

    void reset() {
        slave_pktt.awready = 1;
        slave_pktt.wready = 0;
        slave_pktt.arready = 1;
        slave_pktt.rdata = 0;
        slave_pktt.bid = 0;
        slave_pktt.bresp = 0;
        slave_pktt.bvalid = 0;
        slave_pktt.rid = 0;
        slave_pktt.rresp = 0;
        slave_pktt.rlast = 0;
        slave_pktt.rvalid = 0;

        
    }

    //---------------------------------------
    //Slave events
    //---------------------------------------
    sc_event slave_pktt_send_event;

    sc_event slave_write_channel_trigger_event;
    sc_event slave_read_channel_trigger_event;

    sc_event w_slave_pktt_sent_event;
    sc_event r_slave_pktt_sent_event;

    sc_event w_master_pktt_wvalid_posedge_event;
    sc_event w_master_pktt_bready_posedge_event;
    sc_event r_master_pktt_rready_posedge_event;

    int rw_semaphore = 0; // 1 for write
    // 2 for read 

    //---------------------------------------
    //Slave Write  data channel
    //---------------------------------------
    void get_slave_write_channel_inputs() {
        slave_write_channel_enable = 1;
        slave_write_channel_trigger_event.notify();
    }
    void slave_write_channel() {
        while (true) {
            wait(slave_write_channel_trigger_event);
            if (slave_write_channel_enable && (rw_semaphore == 0)) {
                cout << "SLAVE" << SLAVE_ID << ": ----------------------------------------------------------------------------- slave_write start -----------------------------------------------------------------------------" << endl;

                slave_write_channel_enable = 0;
                rw_semaphore = 1;
                //---------------------------------------
                // Slave aw Hanshaking
                //---------------------------------------
                slave_pktt.awready = 0;
                slave_pktt_send_event.notify();
                wait(w_slave_pktt_sent_event);

                write_address = (sc_uint<32>) (master_pktt.awadr.range(31, 2)); //buffer the address, necessary for variable length bursts
                write_start_address = (sc_uint<32>) (master_pktt.awadr.range(31, 2));
                write_length = master_pktt.awlen;
                write_type = master_pktt.awburst;
                strobe = master_pktt.wstrb;

                wait(aclk.posedge_event());
                slave_pktt.awready = 1;
                slave_pktt_send_event.notify();
                wait(w_slave_pktt_sent_event);
                //---------------------------------------
                // Slave aw Hanshaking DONE 
                //---------------------------------------

                for (int i = 0; i < 16; i++) {
                    extended_strobe = (i % 4 == 3) ? strobe.range(3, 3) :
                        (i % 4 == 2) ? strobe.range(2, 2) :
                        (i % 4 == 1) ? strobe.range(1, 1) :
                        strobe.range(0, 0);
                }
                slave_pktt.wready = 1;
                slave_pktt_send_event.notify();
                wait(w_slave_pktt_sent_event);

                if (master_pktt.wvalid == 0) {
                    wait(w_master_pktt_wvalid_posedge_event);
                }

                for (int i = 0; i <= write_length; i++) {

                    transmitting_data = master_pktt.wrdata.range(31, 0);
                    tx_start = 1;
                    while (tx_busy.read() == 1) {
                        wait();
                    }
                    write_address = write_address + 1;
                    tx_start = 0;
                   
                    wait(aclk.posedge_event());// delete?
                }
                slave_pktt.wready = 0;
                slave_pktt_send_event.notify();
                wait(w_slave_pktt_sent_event);

                //---------------------------------------
                //Slave Write response channel Hanshaking
                //---------------------------------------
                slave_pktt.bresp = AXI_RESPONSE_OKAY;
                slave_pktt_send_event.notify();
                wait(w_slave_pktt_sent_event);

                wait(aclk.posedge_event());
                slave_pktt.bvalid = 1;
                slave_pktt_send_event.notify();
                wait(w_slave_pktt_sent_event);

                if (master_pktt.bready == 0) {
                    wait(w_master_pktt_bready_posedge_event);
                }

                cout << "SLAVE" << SLAVE_ID << ": SLAVE RESPONSE GOOD" << endl;
                wait(aclk.posedge_event());
                slave_pktt.bvalid = 0;
                slave_pktt.bresp = 0;

                //------------------------------------------------
                //Slave Write response channel Hanshaking DONE
                //------------------------------------------------

                extended_strobe = 0;
                strobe = 0;
                write_type = 0;
                write_length = 0;
                write_address = 0;
                write_start_address = 0;
                rw_semaphore = 0;
                cout << "SLAVE" << SLAVE_ID << ": ----------------------------------------------------------------------------- slave_write done -----------------------------------------------------------------------------" << endl;
            }
        }
    }
    //---------------------------------------
    //Slave Read data channel
    //---------------------------------------

    void get_slave_read_channel_inputs() {
        slave_read_channel_enable = 1;
        slave_read_channel_trigger_event.notify();
    }
    void slave_read_channel() {
        while (true) {
            wait(slave_read_channel_trigger_event);
            if (slave_read_channel_enable && (rw_semaphore == 0)) {
                cout << "SLAVE" << SLAVE_ID << ": ----------------------------------------------------------------------------- slave_read start -----------------------------------------------------------------------------" << endl;

                slave_read_channel_enable = 0;
                rw_semaphore = 2;
                //---------------------------------------
                //Slave ar_handshaking
                //---------------------------------------
                read_address = (0b00, master_pktt.araddr.range(31, 2));
                read_start_address = (0b00, master_pktt.araddr.range(31, 2));
                read_length = master_pktt.arlen;
                read_type = master_pktt.arburst;

                slave_pktt.arready = 0;
                slave_pktt_send_event.notify();
                wait(r_slave_pktt_sent_event);
                wait(aclk.posedge_event());

                slave_pktt.arready = 1;
                slave_pktt_send_event.notify();
                wait(r_slave_pktt_sent_event);
                //---------------------------------------
                //Slave ar_handshaking done
                //---------------------------------------

                slave_pktt.rvalid = 1;

                if (master_pktt.rready == 0) {
                    wait(r_master_pktt_rready_posedge_event);
                }

                for (int i = 0; i <= read_length; i++) {
                    while (rx_busy == 1) {
                        wait();
                    }
                    slave_pktt.rdata = received_data;
                    slave_pktt_send_event.notify();
                    wait(r_slave_pktt_sent_event);

                    if (read_type != AXI_BURST_TYPE_FIXED) {
                        read_address = read_address + 1;
                        if ((read_type == AXI_BURST_TYPE_WRAP) && (read_address >= (read_start_address + write_wrap_boundary))) {
                            read_address = read_start_address;
                        }
                    }
                    wait(aclk.posedge_event());
                }
                read_type = 0;
                read_length = 0;
                read_address = 0;
                read_start_address = 0;
                //---------------------------------------
                //Slave rresponse_handshaking
                //---------------------------------------
                slave_pktt.rlast = 1;
                slave_pktt.rresp = AXI_RESPONSE_OKAY;
                slave_pktt_send_event.notify();
                wait(r_slave_pktt_sent_event);
                wait(aclk.posedge_event()); //delete

                slave_pktt.rlast = 0;
                slave_pktt.rvalid = 0;

                //---------------------------------------
                //Slave rresponse_handshaking done 
                //---------------------------------------

                cout << "SLAVE" << SLAVE_ID << ": ----------------------------------------------------------------------------- SLAVE READ DONE -----------------------------------------------------------------------------" << endl;
                rw_semaphore = 0;

            }
        }
    }

    void main_thread() {
        while (true) {
            wait(aclk.posedge_event());
            if (!aresetn.read()) {
                reset();
            }
            else if (rw_semaphore == 0 && master_pktt.awvalid == 1) {
                get_slave_write_channel_inputs();
            }
            else if (rw_semaphore == 0 && master_pktt.arvalid == 1) {
                get_slave_read_channel_inputs();
            }
        }
    }


    //---------------------------------------
    // SEND SLAVE PACKET THROUGH SOCKET
    //---------------------------------------
    void send_slavepkt() {
        // TLM-2 generic payload transaction, reused across calls to b_transport
        tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
        sc_time delay = sc_time(10, SC_NS);

        // Set the payload fields
        trans->set_address(SLAVE_ID);
        trans->set_data_length(sizeof(slave_pkt));
        trans->set_data_ptr(reinterpret_cast<unsigned char*>(&slave_pktt));
        trans->set_command(tlm::TLM_WRITE_COMMAND);

        // Send the payload through the TLM port
        slavepkt_socket->b_transport(*trans, delay);
        if (rw_semaphore == 1) {
            w_slave_pktt_sent_event.notify();
        }
        if (rw_semaphore == 2) {
            r_slave_pktt_sent_event.notify();
        }

        if (trans->is_response_error())
            SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

    }

    //---------------------------------------
    // RECEIVE MASTER PACKET THROUGH SOCKET
    //---------------------------------------
    virtual void readmasterpkt(tlm::tlm_generic_payload & trans, sc_time & delay)
    {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    adr = trans.get_address();
        unsigned char* ptr = trans.get_data_ptr();
        master_pktt = *reinterpret_cast<master_pkt*>(trans.get_data_ptr());
        unsigned int     len = trans.get_data_length();
        unsigned char* byt = trans.get_byte_enable_ptr();
        unsigned int     wid = trans.get_streaming_width();

        if (rw_semaphore == 1) {
            if (master_pktt.wvalid == 1) {
                w_master_pktt_wvalid_posedge_event.notify();
            }
            if (master_pktt.bready == 1) {
                w_master_pktt_bready_posedge_event.notify();
            }
        }
        if (rw_semaphore == 2) {
            if (master_pktt.rready == 1) {
                r_master_pktt_rready_posedge_event.notify();
            }
        }

        // Obliged to set response status to indicate successful completion
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }

    //---------------------------------------
    //SLAVE CONSTRUCTOR
    //---------------------------------------
    SC_CTOR(UART_BFM_SLAVE) : masterpkt_socket("masterpkt_socket"), slavepkt_socket("slavepkt_socket"), uart_controller("uart") {
        // Connect signals
        uart_controller.aclk(aclk);
        uart_controller.aresetn(aresetn);
        uart_controller.tx_start(tx_start);
        uart_controller.transmitting_data(transmitting_data);
        uart_controller.tx(tx);
        uart_controller.tx_busy(tx_busy);
        uart_controller.rx(rx);
        uart_controller.rx_busy(rx_busy);
        uart_controller.received_data(received_data);

        tx_start = 0;
       

        SC_THREAD(init_thread);

        SC_METHOD(send_slavepkt);
        sensitive << slave_pktt_send_event;
        dont_initialize();
        masterpkt_socket.register_b_transport(this, &UART_BFM_SLAVE::readmasterpkt);

        SC_THREAD(main_thread);
        sensitive << aclk;
        dont_initialize();

        SC_THREAD(slave_write_channel);
        sensitive << slave_write_channel_trigger_event << aclk.pos() << w_slave_pktt_sent_event << w_master_pktt_wvalid_posedge_event << w_master_pktt_bready_posedge_event;

        SC_THREAD(slave_read_channel);
        sensitive << slave_read_channel_trigger_event << aclk.pos() << r_slave_pktt_sent_event << r_master_pktt_rready_posedge_event;

    }

};
