// UART_MODEL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// do i need parity?
#include "INCLUDES.h"
#include "UART_RECEIVER.cpp"
#include "UART_TRANSMITTER.cpp"
#include "BAUD_GENERATOR.cpp"
template<int DATA_WIDTH, int baud_rate, int main_clk_period>
SC_MODULE(UART_CONTROLLER) {


    /// common ports

    sc_in<bool> aclk;
    sc_in<bool> aresetn;

    
    sc_signal<bool> baud_clk_out;
    
    /// baud generator ports
    
    /// transmitter ports
    sc_in<bool> tx_start;
    sc_in <sc_uint<DATA_WIDTH>> transmitting_data;

    sc_out<sc_uint<1>> tx;  

    sc_out<bool> tx_busy;

    /// receiver ports 
    sc_in<sc_uint<1>> rx;
    sc_out<bool> rx_busy;

    sc_out <sc_uint<DATA_WIDTH>> received_data;

    BAUD_GENERATOR<baud_rate, main_clk_period> baud_gen_inst;
    UART_TRANSMITTER <DATA_WIDTH> transmitter_inst;
    UART_RECEIVER<DATA_WIDTH> receiver_inst;


    /// 

    //write: transmit


    //read: receive

    //while tx busy (){
    // wait();
    // }
    SC_CTOR(UART_CONTROLLER): baud_gen_inst("baud_gen_inst"), transmitter_inst("transmitter_inst"), receiver_inst("receiver_inst") {

        baud_gen_inst.clk(aclk);
        baud_gen_inst.aresetn(aresetn);
        baud_gen_inst.clk_out(baud_clk_out);

        transmitter_inst.aclk(aclk);
        transmitter_inst.aresetn(aresetn);
        transmitter_inst.tx_start(tx_start);
        transmitter_inst.transmitting_data(transmitting_data);
        transmitter_inst.tx(tx);
        transmitter_inst.tx_busy(tx_busy);

        receiver_inst.aclk(aclk);
        receiver_inst.aresetn(aresetn);
        receiver_inst.rx(rx);
        receiver_inst.rx_busy(rx_busy);
        receiver_inst.received_data(received_data);
    
    }

    
};

