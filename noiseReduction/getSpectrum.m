function [ F] = getSpectrum( f )
%GETSPECTRUM get spectrum in frequency domain move DC to center
    [P,Q]=size(f);
    %step1: padding
    g=[f;zeros(P,Q)];
    g=[g zeros(2*P,Q)];
    [P,Q]=size(g);
    %step2: shift
    x = 0 : P-1;
    y = 0 : Q-1;
    [Y,X]=meshgrid(y,x);
    cshift = (ones(P, Q)*(-1)) .^(X+Y);
    g2 = g .* cshift;
    %DTF 
    F = fft2(g2);
end

