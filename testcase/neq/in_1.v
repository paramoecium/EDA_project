module top (a,b,c,d,o);
input a,b,c,d;
output o;
wire e,f,g,h;
not  (e,b);
and  (f,a,e);
and  (g,b,c);
xor  (h,f,g);
and  (o,h,d);
endmodule
