#include "INCLUDES.h"

// PARITY? 

template<int DATA_WIDTH = 8>
SC_MODULE(UART_TRANSMITTER) {

    sc_in<bool> aclk;
    sc_in<bool> aresetn;
    sc_in<bool> tx_start;
    sc_in <sc_uint<DATA_WIDTH>> transmitting_data;

    sc_out<sc_uint<1>> tx;
    sc_out<bool> tx_busy;

    sc_uint<DATA_WIDTH> tx_FIFO;
    sc_uint<1> tx_bit;


    void reset() {
        while (true) {
            wait();
            if (!aresetn) {
                // TODO: RESET BEHAVIOUR
                tx_busy = 0;
                tx_FIFO = 0;
                tx = IDLE_BIT;
            }
        }
    }

    void transmit() {
        while (true) {
            wait();
            if (aresetn && tx_start) {
                tx_FIFO = transmitting_data;
                // send start bit 
                tx_busy = 1;
                tx = START_BIT;
                wait();
                for (int i = 0; i < DATA_WIDTH; i++) {
                    // shift and send bit by bit
                    tx_bit = tx_FIFO[0];
                    tx = tx_bit;
                    tx_FIFO = tx_FIFO >> 1; // shitft to the right, lsb first 
                    wait();
                }
                //SEND STOP BIT
                tx = STOP_BIT;
                wait();
                tx_busy = 0;
                tx_FIFO = 0;
                tx = IDLE_BIT;
            }
        }

    }

    SC_CTOR(UART_TRANSMITTER) {
        tx_busy = 0;
        tx_FIFO = 0;
        tx = IDLE_BIT;

        SC_THREAD(reset);
        sensitive << aclk.pos();

        SC_THREAD(transmit);
        sensitive << aclk.pos();
    }


};