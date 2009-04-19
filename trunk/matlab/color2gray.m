% File: color2gray.m
% Author: ryanlei
% Creation: 2009/04/19
% Modification: XXXX/XX/XX
% Description: Transform a 3D color image into a 2D grayscale.

function [ result ] = color2gray ( img )
result = img(:,:,1) / 3 + img(:,:,2) / 3 + img(:,:,3) / 3;
end