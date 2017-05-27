function [ F ] = myDFT2( f )
%MYDFT2 Summary of this function goes here
%   Detailed explanation goes here
    [P,Q] = size(f);
    x = 0 : P-1;
    y = 0 : Q-1;
    [Y,X]=meshgrid(y,x);

    F = single(ones(P, Q));
    for u = 1 : P
        for v = 1 : Q
            F(u,v) = sum(sum(f .* exp(-1j*2*pi*(u/P*X + v/Q*Y))));
        end
    end
end

