function [ f ] = myIDFT2( F )
%MYIDFT Summary of this function goes here
%   Detailed explanation goes here

    [P,Q]=size(F);   
    u = 0:P-1;
    v = 0:Q-1;
    [V,U] = meshgrid(v,u);
    f = single(ones(P,Q));
    
    %for x = 1 : P
    %    for y = 1 : Q
    %        f(x,y) = sum(sum(F .* exp(1j*2*pi*(x/P*U + y/Q*V))));
    %    end
    %end

    UV = U*V;
    w1 = mod(UV/P,P);
    w2 = mod(UV/Q,Q)';
    f = exp(-1j*2*pi*w1) * F * exp(-1j*2*pi*w2);
 
    f = f/(P*Q);
end

