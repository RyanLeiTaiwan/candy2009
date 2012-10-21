%%%%%%%% CS5540 Pattern Recognition by Chiou-Ting Hsu
% File name: predict.m
% ID: u9562171
% Author: Lei Yu-Heng (Ryan Lei)
% Start Date: 2010/06/11
% Finish Date: 2010/06/16
% Description: Predict image classes given the libSVM model.

% ShowEach (logical): Show the image and classification result one by one. Default: false.
function predict(ShowEach)
    global class_name model test_directory var_percentage A

    % magnifying ratio of imshow()
    imshow_ratio = 200;

    if nargin == 0
        ShowEach = false;
    end
    
    % l: dimensionality
    % c: number of classes
    % N2: cumulative training data size of each class (c x 1)
    % X2: testing data matrix (N2(c) x l)
    % y2: testing data label (1 x N2(c))
    c = size(class_name,1);

    %%% Read visual word histograms from files
    %%% The files should be in "test_directory", and named "VW_test_XXXX.txt",
    %%% where XXXX should be consistent with the ones defined in class_name
    fprintf('Read the visual word histograms from files ...\n');
    tic;
    X2 = [];
    y2 = [];
    N2 = [];
    for i = 1 : c
        read = load([test_directory,'/VW_test_',char(class_name{i}),'.txt']);
        X2 = [X2; read];
        y2 = [y2; i * ones(size(read,1),1)];
        N2(i) = size(X2,1);
        fprintf('  %s finished\n',[test_directory,'/VW_test_',char(class_name{i}),'.txt']);
    end
    toc;
    
    
    %%% PCA for transformation / dimension reduction if the user previously specified in train
    %%% Use the same transformation A
    if var_percentage < 100
        fprintf('### PCA: retaining %g%% of total variance ...\n',var_percentage);
        fprintf('###    reduced dimensionality = %g\n',size(A,2));
        X2 = X2 * A;
    end

    
    % Display class names
    fprintf('\nclass names:\n');
    for i = 1 : c
        fprintf('  %g    %s\n',i,class_name{i});
    end
    
    %%% Call svmpredict and compute the confusion matrix
    fprintf('\nPerform libSVM predict ...\n');
    tic;
    y2_predict = svmpredict(y2,X2,model);
    toc;    
    % confusion: the confusion matrix used in "performance evaluation"
    confusion = zeros(c,c);
    for i = 1 : N2(c)
        confusion(y2(i),y2_predict(i)) = confusion(y2(i),y2_predict(i)) + 1;
    end        

    % Performance evaluation:
    % Show the confusion matrix
    fprintf('\nConfusion matrix:\n\n');
    disp(confusion);
    % Compute and show the "recall" and "precision" of each class
    recall = diag(confusion) ./ sum(confusion,2);
    fprintf('Recall for each class:\n\n');
    disp(recall);
    fprintf('Precision for each class:\n\n');
    precision = diag(confusion) ./ sum(confusion,1)';
    disp(precision);

    
    % If user chooses to show the images one by one:
    if ShowEach == true
        %%% read the file names
        %%% The lists should be named "list_test_XXXX.txt" in "test_directory", where XXXX
        %%% should be consistent with the ones defined in class_name of train().
        filename = {};
        for i = 1 : c
            filename = [filename;importdata([test_directory,'/list_test_',char(class_name{i}),'.txt'])];
        end
        
        figure;
        % Randomly decide the imshow() order
        for i = randperm(N2(c))        
            % class that i belongs to
            class = sum(i > N2) + 1;
            file = [test_directory,'/',char(class_name{class}),'/',char(filename{i})];
            imshow(file,'InitialMagnification',imshow_ratio);
            % get image height without imread()
            info = imfinfo(file);
            height = info.Height;
            % Put text of the predicted label, 'O' or 'X' for correctness
            text(0,0,class_name(y2_predict(i)),'Color',[1,1,0],'BackgroundColor',[0,0,1],'FontSize',36);
            if y2_predict(i) == y2(i)
                text(0,height / 2,'O','Color',[0,1,0],'BackgroundColor',[0,0,1],'FontSize',72);
            else
                text(0,height / 2,'X','Color',[1,0,0],'BackgroundColor',[0,0,1],'FontSize',72);
            end
            pause;
        end        
    end
    
end
