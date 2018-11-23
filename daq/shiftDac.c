#define rg 0
#define pg 1

#define dacReg(val) do {                                   \
       config_reg(rg,(val>>16) & 0xffff);     \
	   pulse_reg(pg);  \
       config_reg(rg,    val & 0xffff);     \
	   pulse_reg(pg);  \
    } while (0);
// while(1){
//HARDRST
dacReg(0x0a000000);
// }
//SOFTRST
dacReg(0x07000000);

//REFON
dacReg(0x090a0000);

//POWERCH
dacReg(0x040000ff);

    uint32_t *buf32, val;
    size_t n;
    buf32 = (uint32_t*)buf;
    /* DAC8568 for TopmetalII- */
    /* turn on internal vref = 2.5V, so the output is val/65536.0 * 5.0 [V] */
#define DACVolt(x) ((uint16_t)((double)(x)/5.0 * 65536.0))
    // n = cmd_write_register(&buf32, 0, 0x0800);
    // n = query_response(sockfd, buf, n, buf, 0);
    // n = cmd_send_pulse(&buf32, 0x02); /* pulse_reg(1) */
    // n = query_response(sockfd, buf, n, buf, 0);
    // n = cmd_write_register(&buf32, 0, 0x0001);
    // n = query_response(sockfd, buf, n, buf, 0);
    // n = cmd_send_pulse(&buf32, 0x02); /* pulse_reg(1) */
    // n = query_response(sockfd, buf, n, buf, 0);
    /* write and update output1 : DAC_EX_VREF */
    val = (0x03<<24) | (0x00 << 20) | (DACVolt(2.5) << 4);
    n = cmd_write_register(&buf32, 0, (val & 0xffff0000)>>16);
    n = query_response(sockfd, buf, n, buf, 0);
    n = cmd_send_pulse(&buf32, 0x02); /* pulse_reg(1) */
    n = query_response(sockfd, buf, n, buf, 0);
    n = cmd_write_register(&buf32, 0, val & 0xffff);
    n = query_response(sockfd, buf, n, buf, 0);
    n = cmd_send_pulse(&buf32, 0x02); /* pulse_reg(1) */
    n = query_response(sockfd, buf, n, buf, 0);

    return 1;
#undef DACVolt
#undef rg
#undef pg
#undef dacReg