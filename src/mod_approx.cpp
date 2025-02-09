int calc_k(const int shr, const int b)
{
    //return 1 + ( (1 << shr) / b );
    return ( (1 << shr) + b - 1 ) / b ;
}

int a_mod_b_approx(const int16_t a, const int16_t b, const int16_t k, const int16_t shr)
{
    // mod = a - ( (a * k) / 2^x ) * b

    const int tmp1 = a * k;
    const int tmp2 = tmp1 >> shr;
    const int tmp3 = tmp2 * b;
          int tmp4 = a - tmp3;

    if ( tmp4 < 0 ) {
        tmp4 += b;
    }
    
    return tmp4;
}

void run_simple() {
    const int b_max = 3;
    const int a_max = 10;
    const int shr_max = 16;

    size_t count_diff = 0;
    size_t sum_delta = 0;

    for (int shr=16; shr <= shr_max; shr++) {
        for (int b=1; b <= b_max; b++) {

            const int k = calc_k(shr,b);

            count_diff = 0;
            sum_delta = 0;
            for (int a = 0; a <= a_max; a++) {

                const int mod_approx   = a_mod_b_approx(a,b,k,shr);
                const int mod_expected = a % b;

                if ( mod_expected != mod_approx ) {
                    count_diff += 1;
                    const int delta = abs(mod_approx - mod_expected);
                    sum_delta += delta;
                    printf("E: shr: %d\ta: %d\tb: %d\texpected: %d != \tgot: %d\n", shr, a, b, mod_expected, mod_approx);
                }
                else {
                    printf("I: shr: %d\ta: %d\tb: %d\texpected: %d == \tgot: %d\n", shr, a, b, mod_expected, mod_approx);
                }
            }
            //if ( count_diff > 0) {
                //printf("shr: %d\tb: %d\tdiffs: %ld\tsum_delta: %ld\n", shr, b, count_diff, sum_delta);
            //}
            
        }
    }
}
