static const int NUM_MULT = 9;

class processing_element {

private:
  float products[NUM_MULT];
  float acc;

public:
  /** Multiply-Accumulate Unit **/
  void macc(float img_in[NUM_MULT], float filt_in[NUM_MULT], float &sum_out,
            float prod_out[NUM_MULT]);
};
