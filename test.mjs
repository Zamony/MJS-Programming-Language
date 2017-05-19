#! ./cgi-bin/mjs_bin
#

var x, y;

x = 64;
y = 4;

#
y = 7;
#

write((x + y*2 - 2)/2+7); # 42 #

if ( x > y ){
  write(x);
} else write(y);

if ( y < x)
  write(y);

write(x / y);

var i = 0;
while (i < 5){
write(i);
i = i + 1;
}

i = 5;
do {
write(i);
i = i - 1;
} while (i > 0);

write(7 * 111);

for (i = 1; i < 25; i = i + 2){
 write(i);
}

@
