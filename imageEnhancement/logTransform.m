function [ output ] = logTransform( input )
% intensity transform by log function
%   s = clog(1+r) c = log(2) map s into [0,1]
%   input & output [0,1] single
    output = 1/log(2)*log(1+input);
end

