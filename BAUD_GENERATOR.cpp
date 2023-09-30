
#include "INCLUDES.h"
template<int baud_rate, int main_clk_period>
SC_MODULE(BAUD_GENERATOR) {
    sc_in<bool> clk;
    sc_in<bool> aresetn;
    sc_out<bool> clk_out;

    int cycles_count;
    int counter;
    bool c_out;

    int main_clk_period_in_secs;
    double baud_clk_period;
     //int cycles_per_baud = static_cast<int>(baud_clk_period / (main_clk_period * 1e-9));

    void generate_baud_clk() {
        counter = 0;
        c_out = false;
        clk_out.write(c_out);
        while (true) {
            wait();
            counter++;
            if (counter >= cycles_count) {
                c_out = !c_out;
                clk_out.write(c_out);
                counter = 0;
            }

        }

    }

    void calculate_cycles_per_bit(){ 

        // convert main clk period from ns to s
        main_clk_period_in_secs = main_clk_period * 1e-9;
        // calculate baud clk period
        baud_clk_period = 1 / baud_rate;
        
        cycles_count = static_cast<int>(baud_clk_period / main_clk_period);
    }


    SC_CTOR(BAUD_GENERATOR) {
        
        SC_THREAD(calculate_cycles_per_bit);
        SC_THREAD(generate_baud_clk);
        sensitive << clk.pos();
        dont_initialize();
    }
};

