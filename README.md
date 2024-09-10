# Quine-McCluskey

此儲存庫用於存放2022年初的計概期末Project Bonus，使用Quine-McCluskey and Petrick’s Method實現的logic minimization程式。
該項目使用C++11實現，若要使用請自行編譯，所有原始程式碼皆於main.cpp中

使用方式:
啟動程式並輸入測試檔案相對路徑 如testcase/test1.eqn 將在測試檔案同資料夾下生成對應test1out.eqn

測試檔案格式為:
INORDER = a b c d; // 指定使用的化簡符號(小寫字母a~z)
OUTORDER = output;
output = (!a*!b*!c*!d)+(!a*b*!c*d)+(!a*b*c*!d)+(a*!b*!c*d)+(a*!b*c*!d)+(a*b*!c*d)+(a*b*c*!d)+(a*b*c*d)+(!a*b*c*d) 
// 可使用"!"表達complement、"+"表達OR、"*"表達AND、可使用()進行組合