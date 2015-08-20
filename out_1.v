module top ( a , b , c , o );
input a , b , c;
output o;
wire d , e , f , f_cut , g , g_cut;
xor ( o , f , g );
and ( d , a , b );
xor ( e , a , b );
_cut cut_2_1 ( f , f_cut );
xor ( f_cut , d , e );
_cut cut_1_1 ( g , g_cut );
nand ( g_cut , b , c );
endmodule
