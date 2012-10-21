%%%%%%%% CS5540 Pattern Recognition by Chiou-Ting Hsu
% File name: train.m
% ID: u9562171
% Author: Lei Yu-Heng (Ryan Lei)
% Start Date: 2010/06/11
% Finish Date: 2010/06/16
% Description: 1. Read visual word data from files.
%              2. (Optional) Run PCA for dimension reduction.
%              3. Train using libSVM, or write data to a file without training.

% write_file (logical): meaning "DON'T TRAIN, but write to file in libSVM format". Default: false
%%% var_percentage_in: percentage of total variance retained in PCA. 
%%% Range: [0,100]. Default: 100 (no PCA).
%%% var_percentage_in = 0 means directly using the previous transformation A without recomputing PCA.
% C: parameter C (cost) in libSVM. Default: 1
% gamma: parameter gamma in libSVM. Default: 1 / l
function train(write_file,var_percentage_in,C,gamma)
    % model: multi-class SVM model trained by libSVM
    % A: the transformation matrix A in PCA
    global class_name model test_directory var_percentage A
    
    %%% Check user command
    if nargin == 0
        write_file = false;
        var_percentage = 100; % no PCA
        C = 1;
        gamma = -1; % temporary, because we don't know "l" yet
    elseif nargin == 1
        if islogical(write_file) == false
            fprintf('Error: write_file must be ''true'' or ''false''\n');
            return;
        end
        var_percentage = 100; % no PCA
    elseif nargin == 2
        if islogical(write_file) == false
            fprintf('Error: write_file must be ''true'' or ''false''\n');
            return;
        end
        if var_percentage_in < 0 || var_percentage_in > 100
            fprintf('Error: PCA''s total variance percentage should be within [0,100]\n');
            return;
        end
        if var_percentage_in ~= 0
            var_percentage = var_percentage_in; % set the global variable var_percentage to argument 2
        end
        C = 1;
        gamma = -1; % temporary, because we don't know "l" yet
    elseif nargin == 3
        if islogical(write_file) == false
            fprintf('Error: write_file must be ''true'' or ''false''\n');
            return;
        end
        if write_file == true
            fprintf('Error: C should be used with 1st argument = ''false'' to make sense\n');
            return;
        end
        if var_percentage_in < 0 || var_percentage_in > 100
            fprintf('Error: PCA''s total variance percentage should be within [0,100]\n');
            return;
        end
        if var_percentage_in ~= 0
            var_percentage = var_percentage_in; % set the global variable var_percentage to argument 2
        end
        C = 1;
        gamma = -1; % temporary, because we don't know "l" yet
    elseif nargin == 4
        if islogical(write_file) == false
            fprintf('Error: write_file must be ''true'' or ''false''\n');
            return;
        end
        if write_file == true
            fprintf('Error: C, gamma should be used with 1st argument = ''false'' to make sense\n');
            return;
        end
        if var_percentage_in < 0 || var_percentage_in > 100
            fprintf('Error: PCA''s total variance percentage should be within [0,100]\n');
            return;
        end
        if var_percentage_in ~= 0
            var_percentage = var_percentage_in; % set the global variable var_percentage to argument 2
        end
    end

    
    % [0] Program settings
    %%% Class names should be consistent with the visual word file names (XXX.txt)
    class_name = {'Airplane';'Face';'Motorbike';'None'};

    %%% Set the input image direcoty
    %%% The subdirectory names of each class should be consistent with 
    %%%   "class_name" set in train.m.
    train_directory = 'Caltech_101_train';
    test_directory = 'Caltech_101_test';
    output_directory = '../libsvm-2.91/tools';  % libSVM output format to be used with grid.py
    output_filename = 'VW_train';
    % add the MATLAB interface of libSVM to PATH
    addpath '../libsvm-mat-2.91-1';

    % l: dimensionality
    % c: # of classes
    % N1: cumulative training data size of each class (c x 1)
    % X1: training data matrix (N1(c) x l)
    % y1: training data label (1 x N1(c))
    c = size(class_name,1);

    %%% Read visual word histograms from files
    %%% The files should be in "train_directory", and named "VW_train_XXXX.txt",
    %%% where XXXX should be consistent with the ones defined in class_name
    fprintf('Read the visual word histograms from training files ...\n');
    tic;
    X1 = [];
    y1 = [];
    N1 = 0;
    for i = 1 : c
        read = load([train_directory,'/VW_train_',char(class_name{i}),'.txt']);
        X1 = [X1; read];
        y1 = [y1; i * ones(size(read,1),1)];
        N1(i) = size(X1,1);
        fprintf('  %s finished\n',[train_directory,'/VW_train_',char(class_name{i}),'.txt']);
    end
    toc;
    
    
    %%% Directly use a previously computed PCA transformation matrix. Be careful!
    if nargin >= 2 && var_percentage_in == 0
        fprintf('\n### [CAUTION] Use a previously computed PCA transform ...\n');
        fprintf('### PCA: retaining %g%% of total variance ...\n',var_percentage);
        X1 = X1 * A;
        fprintf('###    reduced dimensionality = %g\n\n',size(X1,2));
        
    %%% PCA for dimensionality reduction (transformation) if the user specified
    elseif var_percentage < 100
        fprintf('\n### PCA: retaining %g%% of total variance ...\n',var_percentage);
        % pc: the principal components in column vectors
        % var: variance (eigenvalue) in each pc direction
        % exp: total variance explained by this pc
        tic;
        [pc,var,exp] = pcacov(cov(X1));        
        % k: the reduced dimensionality, determinced by the requirement that we want to
        % retain "var_percentage %" of the total variance
        k = sum(cumsum(exp) < var_percentage) + 1;
        fprintf('###    reduced dimensionality = %g\n\n',k);
        % the A matrix for transformation
        A = pc(:,1:k);
        % actual dimensionality reduction
        X1 = X1 * A;
        toc;
    end

    % Record the (may have been reduced) dimensionality for computing default gamma
    l = size(X1,2);
    % Convert X1 to a sparse matrix
    X1 = sparse(X1);
    
    % If not writing to file => directly run svmtrain
    if write_file == false
        %%% set gamma = 1 / l if not specified (previously -1)
        if gamma == -1
            gamma = 1 / l;
        end
        tic;
        fprintf('\nPerform libSVM train ...\n')
        model = svmtrain(y1,X1,['-c ',char(num2str(C)),' -g ',char(num2str(gamma))]);
        toc;
    % If writing to file => call libsvmwrite()   
    else
        tic;
        fprintf('\nWrite to file "%s" in libSVM format ...\n',[output_directory,'/',output_filename]);
        libsvmwrite([output_directory,'/',output_filename],y1,X1);
        toc;
    end
    
    
end
