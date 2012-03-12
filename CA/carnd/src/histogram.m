function [h, ent] = compute_all (filename, n, beans)
  f = fopen(filename, "rb", "ieee-le");
  x = fread (f, n, "float", 0);
  [h, ent] = hist(x, beans);
  fclose(f);
endfunction;

function [suma] = count_gt (filename, n, umbral)
  f = fopen(filename, "rb", "ieee-le");
  x = fread (f, n, "float", 0);
  fclose(f);

  suma = 0;
  for i = 1:n;
    if (x(i) >= umbral);
      suma = suma + 1;
    endif;
  endfor;
endfunction;

function computeone (filename, N, beans)
  [h, ent] = compute_all('15-10M-1024-12-0', 10000000, 100);
endfunction;

%for i = 900:1000;
%  printf ("%f %f\n", ent(i), h(i));
%endfor;

% http://www.arcknowledge.com/gnu.octave.bugs/2006-01/msg00014.html
function xgset (s)
 evalin ("base", sprintf ("__gnuplot_set__ %s", s));
endfunction


function show (filename)
 [h, ent] = compute_all(filename, 10000000, 100);

  xgset("term png");
  xgset (strcat("output ", '"', filename , "-todo.png", '"'));
  plot(ent,h);

  xgset("term png");
  xgset (strcat("output ", '"', filename , "-last10pc.png", '"'));
  plot(ent(90:100), h(90:100));
endfunction;

show('./15-10M-1024-255-0');
show('./15-10M-1024-256-0');
show('./15-10M-1024-257-0');
