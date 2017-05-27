function [ output ] = powerlawTransform( input, r )
% power law transform
%   g = cf^r, c = 1
%   input & output [0,1] single
    output = input.^r;
end

