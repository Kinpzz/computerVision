function [ output ] = imageHist( input )
% get the hisogram of a photo
%   input uint8 [0~255]
    output = single(zeros(1,256));
    for i = 1 : size(input,1)
        for j = 1 : size(input,2)
            output(1,input(i,j)+1) = output(1,input(i,j)+1) + 1;
        end
    end
end

