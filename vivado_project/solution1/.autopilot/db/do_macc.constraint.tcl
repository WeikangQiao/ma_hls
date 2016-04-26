set clock_constraint { \
    name clk \
    module do_macc \
    port ap_clk \
    period 10 \
    uncertainty 1.25 \
}

set all_path {}

set false_path {}

