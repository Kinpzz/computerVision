function [ f ] = getSpacial( F )
%GETSPACIAL Summary of this function goes here
%   Detailed explanation goes here
    
    %IDFT
    g = real(ifft2(F));
    %shift back
    P = size(F,1);
    Q = size(F,2);
    x = 0 : P-1;
    y = 0 : Q-1;
    [Y,X]=meshgrid(y,x);
    cshift = (ones(P, Q)*(-1)) .^(X+Y);
    g = g .* cshift;
    %cut
    f = g(1:size(F,1)/2,1:size(F,2)/2);

end

