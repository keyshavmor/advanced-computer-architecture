figure();
bar(task3orgaL1);
grid on;
title('L1 miss rates')
ylabel('miss rates')
ax = gca;
ax.XAxis.TickLabels = {'both unified','L1 split, L2 unified','both split'};
legend('fibonacci','matmul','memcopy','pi','whetstone','Location','NorthWest');

figure();
bar(task3orgaL2);
grid on;
title('L2 miss rates')
ylabel('miss rates')
ax = gca;
ax.XAxis.TickLabels = {'both unified','L1 split, L2 unified','both split'};
legend('fibonacci','matmul','memcopy','pi','whetstone','Location','NorthWest');