function [ output ] = myGHPF(  D0, M, N  )
%MYGHPF Summary of this function goes here
%   Detailed explanation goes here
    Hlp = myGLPF(D0, M, N);
    output = 1 - Hlp;
end

