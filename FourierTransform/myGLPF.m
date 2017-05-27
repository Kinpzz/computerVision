function [ output ] = myGLPF( D0, M, N )
%MYGLPF Summary of this function goes here
%   D0: cut off frequency in GLPF, type single
%   M, N: size of the filter(M x N), type int
%   output: a 2-D GLPF, type single
%   the center of GLPF is in the central
%   H(u,v) = exp(-D(u,v)^2/2D0^2)

    d = 2*D0^2;
    x = [floor((M-1)/2) :-1: 0, 1 : ceil((M-1)/2)];
    y = [floor((N-1)/2) :-1: 0, 1 : ceil((N-1)/2)];
    [Y,X] = meshgrid(y,x);
    S = X.^2 + Y.^2;
    output = single(exp(-S/d));
end

