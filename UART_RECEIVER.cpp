#include "INCLUDES.h"
template<int DATA_WIDTH = 8>
SC_MODULE(UART_RECEIVER) {
    sc_in<bool> aclk;
    sc_in<bool> aresetn;
    sc_in<sc_uint<1>> rx;
    sc_out<bool> rx_busy;

    sc_out <sc_uint<DATA_WIDTH>> received_data;

    sc_uint<8> rx_FIFO;
    State current_state;

    void reset() {
        while (true) {
            wait();
            if (!aresetn) {
                // TODO: RESET BEHAVIOUR
                rx_FIFO = 0;
                received_data = 0;
                rx_busy = 0;
                current_state = IDLE;

            }
        }
    }

    void receive() {
        while (true) {
            wait();
            if (aresetn) {
                if (current_state == IDLE) {
                    if (rx.read() == START_BIT) {
                        rx_busy = 1;
                        current_state = RECEIVING;
                        wait();
                    }
                }
                else if (current_state == RECEIVING) {
                    for (int i = 0; i < 8; i++) {
                        // TODO: IMPLEMENT SHIFT
                        rx_FIFO[i] = rx.read();
                        wait();
                    }
                    current_state = STOP;
                }
                else {
                    while (rx.read() != STOP_BIT) {
                        wait();
                    }
                    received_data = rx_FIFO;
                    wait();
                    rx_FIFO = 0;
                    rx_busy = 0;
                    current_state = IDLE;

                }

                
            }
        }

    }

    SC_CTOR(UART_RECEIVER) {
        rx_FIFO = 0;
        received_data = 0; 
        rx_busy = 0;
        current_state = IDLE;
       
        SC_THREAD(reset);
        sensitive << aclk.pos();

        SC_THREAD(receive);
        sensitive << aclk.pos();

    }
    

};