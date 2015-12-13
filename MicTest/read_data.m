file = 'file.csv';
data = csvread(file);
plot(1:length(data),data)
title(['Data read from ' file])
xlabel('Sample Number')
ylabel('Analog Voltage Value (0-1023)')