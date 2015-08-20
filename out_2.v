module top ( a , b , c , o );
input a , b , c;
output o;
wire d , e , f , f_cut , g , g_cut;
xnor ( o , f , g );
and ( d , a , b );
xor ( e , a , b );
_cut cut_2_1 ( f , f_cut );
or ( f_cut , d , e );
_cut cut_1_bar_1 ( g , g_cut );
and ( g_cut , b , c );
endmodule
