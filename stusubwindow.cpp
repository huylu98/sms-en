#include "stusubwindow.h"
#include "ui_stusubwindow.h"

StuSubWindow::StuSubWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StuSubWindow)
{
    ui->setupUi(this);
    //成员变量初始化
    this->tb = ui->tableWidget; //Point to the table in the window
    this->filePath = "";        //file path
    this->flagModified = false;  //Has it been modified
    this->stuFilter = new FilterDialog();
    this->stuSorter = new SortDialog();
//    this->printer = new QPrinter();

    //Process some characteristics of the table
    tb->setSelectionBehavior(QAbstractItemView::SelectRows);    //The way to select the entire row
    tb->setSelectionMode (QAbstractItemView::SingleSelection);  //Select a single row (prevent multiple rows from being edited at once)
    tb->setEditTriggers(QAbstractItemView::NoEditTriggers);     //Prohibit directly modifying the cell

    //Bind the signal of the double-click event to the processing function (double-click to change the value)
    connect(tb,SIGNAL(on_tableWidget_cellDoubleClicked(int,int)),this,SLOT(on_tableWidget_cellDoubleClicked(int,int)));

    //Click the title to sort
    QHeaderView *headerGoods = tb->horizontalHeader();
    headerGoods->setSortIndicator(0, Qt::AscendingOrder);
    headerGoods->setSortIndicatorShown(true);
    connect(headerGoods, SIGNAL(sectionClicked(int)), tb, SLOT (sortByColumn(int)));
}

StuSubWindow::~StuSubWindow()
{
    delete ui;
}

void StuSubWindow::addLine()
{
    StuDialog *dialog = new StuDialog();
    if(dialog->exec() == QDialog::Accepted)
    {
        this->flagModified = true;  //The file has been modified and needs to be saved when closed

        QVector<QString> res = dialog->getInput();
        //The ID is the same and cannot be added repeatedly
        QString ID = res[0];
        if(this->primaryKeyConfilct(ID,-1)){
            return;
        }
        //ID does not exist, you can add
        int row_count = tb->rowCount(); //Get the number of form rows
        tb->insertRow(row_count); //Insert new row
        for(int i=0;i<=5;i++){
            QTableWidgetItem *newItem = new QTableWidgetItem();
            newItem->setText(res[i]);
            tb->setItem(row_count,i,newItem);
        }
    }
}

void StuSubWindow::alterLine()
{
    int rowCur = tb->currentRow();
    qDebug()<<"The modified line number is： "<<rowCur<<endl;
    if(rowCur>=0){
        on_tableWidget_cellDoubleClicked(rowCur,0);
    }
}

void StuSubWindow::deleteLine()
{
    this->flagModified = true;  //The file has been modified and needs to be saved when closed
    int rowCur = tb->currentRow();
    qDebug()<<"The deleted line number is： "<<rowCur<<endl;
    if(rowCur>=0){
        tb->removeRow(rowCur);
    }
}

void StuSubWindow::sortByID()
{
    this->flagModified = true;  //The file has been modified and needs to be saved when closed
    tb->sortItems(0);
}

void StuSubWindow::sortByName()
{
    this->flagModified = true;  //The file has been modified and needs to be saved when closed
}

void StuSubWindow::sortByUser()
{
    if(stuSorter->exec()==QDialog::Accepted){
        int col =stuSorter->getSortTarget();
        switch (stuSorter->getSortOrder()) {
            case 0:this->tb->sortItems(col,Qt::AscendingOrder);break;
            case 1:this->tb->sortItems(col,Qt::DescendingOrder); break;
        }
    }
}


/**
   - Qt::MatchFixedString：执行基于字符串的匹配。（基于字符串的比较不区分大小写，除非MatchCaseSensitive：）
   - Qt::MatchContains：搜索项包含在项中。
   - Qt::MatchStartsWith：搜索项与项的开头匹配。
   - Qt::MatchEndsWith：搜索项与项的结尾匹配。
   - Qt::MatchRegExp (overrides all flags above)使用正则表达式作为搜索项执行基于字符串的匹配。
   - Qt::MatchCaseSensitive：大小写敏感
**/

bool StuSubWindow:: QString_Matches(
    const QString& str,
    const QString& pattern,
    const Qt::MatchFlags& flags)
{
    if(flags.testFlag(Qt::MatchRegExp) == true)
    {
        QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
        if(flags.testFlag(Qt::MatchCaseSensitive) == false)
        {
            options = QRegularExpression::CaseInsensitiveOption;
        }
        QRegularExpression regex(pattern, options);
        return regex.match(str).hasMatch();
    }
    else
    {
        Qt::CaseSensitivity cs = Qt::CaseSensitive;
        if(flags.testFlag(Qt::MatchCaseSensitive) == false)
        {
            cs = Qt::CaseInsensitive;
        }
        if(flags.testFlag(Qt::MatchContains) == true)
        {
            return str.contains(pattern, cs);
        }
        else
        {
            if(flags.testFlag(Qt::MatchStartsWith) == true)
            {
                if(str.startsWith(pattern, cs) == true)
                {
                    return true;
                }
            }
            if(flags.testFlag(Qt::MatchEndsWith) == true)
            {
                if(str.endsWith(pattern, cs) == true)
                {
                    return true;
                }
            }
            if(flags.testFlag(Qt::MatchFixedString) == true)
            {
                return (str.compare(pattern, cs) == 0);
            }
        }
    }
    return false;
};

void StuSubWindow::filter()
{
    if(stuFilter->exec() == QDialog::Accepted){

        //Find keywords
        QString text = stuFilter->getKey();

        //Find mode
        Qt::MatchFlags flag;
        QString pattenType = stuFilter->getType();
        if(pattenType=="Partial"){
            flag |= Qt::MatchContains;
        }else if(pattenType == "Prefix"){
            flag |= Qt::MatchStartsWith;
        }else if(pattenType == "Suffix"){
            flag |= Qt::MatchEndsWith;
        }

        //Find accuracy
        if(stuFilter->isRegex()){
            flag |= Qt::MatchRegExp;
        }

        if(stuFilter->isWild()){
            flag |= Qt::MatchWildcard;
        }

        if(stuFilter->isCaseSensitive()){
            flag |= Qt::MatchCaseSensitive;
        }

        //Look in
        QList <QTableWidgetItem *> item;
        if(stuFilter->getTarget()=="All"){
            item = tb->findItems(text, flag);
        }else{
            int index = stuFilter->getTargetIndex()-1;
            qDebug()<<"Selected"<<index<<endl;
            for(int i=0;i<tb->rowCount();i++){
                if(QString_Matches(tb->item(i,index)->text(),text,flag)){
                    item.push_back(tb->item(i,index));
                }
            }
        }


        for (int i = 0; i < tb->rowCount(); i++)
        {
            tb->setRowHidden(i, true);//隐藏所有行
        }

        if (!item.isEmpty())//不为空
        {
            for (int i = 0; i < item.count(); i++)
            {
                tb->setRowHidden(item.at(i)->row(),false);//item.at(i).row()输出行号
            }
        }
    }


}

void StuSubWindow::printTable()
{
//    QPrintDialog dialog(this->printer, this);
//    if(dialog.exec() == QDialog::Rejected) return;
//    //printPreviewAction();
//    QPixmap pixmap(ui->tableWidget->size());
//    ui->tableWidget->render(&pixmap);
//    QPainter painter;
//    painter.begin(printer);
//    painter.drawPixmap(QRectF(0, 0, pixmap.width(), pixmap.height()), pixmap, QRectF(0, 0,pixmap.width(), pixmap.height()));
//    painter.end();

}

bool StuSubWindow::saveFile()
{
    //保存，是已经打开的，或者保存过的文件保存
    if(filePath == ""){
        return this->saveFileAs();
    }else{
        //就把表格里的内容都存到filePath里就好
        return this->saveTableTo(filePath);
    }
}

bool StuSubWindow::saveFileAs()
{
    QFileDialog fileDialog;
    QString fileName = fileDialog.getSaveFileName(this,tr("Open File"),"/New form",tr("Text File(*.txt)"));

    if(fileName == ""){
        return false;
    }
    else{
        QString simpleName = this->getFileNameWithoutFormat(fileName);
        this->setWindowTitle(simpleName);   //“另存为”成功后，将窗口改名
        if(this->filePath == ""){
            this->filePath = fileName;        //初次保存
        }
        return this->saveTableTo(fileName);
    }
}

bool StuSubWindow::saveTableTo(QString filepath)
{
    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,tr("Caveat"),tr("fail to open the file"));
        return false;
    }
    else
    {
        QTextStream textStream(&file);
        textStream.setCodec("UTF-8");
        textStream << FILE_KEY << endl;
        for(int i=0; i <ui->tableWidget->rowCount(); i++)
        {
            for(int j=0; j<=5;j++){
                textStream << ui->tableWidget->item(i,j)->text()<<" ";
            }
            textStream<<endl;
        }
        QMessageBox::information(this,tr("Prompt"),tr("Saved successfully"));
        this->flagModified = false;
        file.close();
        return true;
    }
}

void StuSubWindow::importFile(QString fileName)
{
    this->filePath = fileName;
    QFileInfo fi = QFileInfo(fileName);
    qDebug()<<fi.fileName()<<endl;
    this->windowTitleChanged(fi.fileName());
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,tr("Error"),tr("fail to open the file"));
        return;
    }
    else
    {
        if(!file.isReadable())
        {
            QMessageBox::warning(this,tr("Error"),tr("The file is not readable"));
        }
        else
        {
            //该文件可以成功打开，于是将信息导入到
            QTextStream textStream(&file);
            textStream.setCodec("UTF-8");
            QString lineInfo;
            lineInfo = textStream.readLine();

            //文件标识，系统只能打开特定格式文件
            if(lineInfo != FILE_KEY){
                QMessageBox::warning(this,tr("Error"),tr("The file is not compatible with the system"));
                this->close();
                return;
            }
            while(!textStream.atEnd())
            {
                lineInfo = textStream.readLine();
                QStringList newLine = lineInfo.split(QRegExp("[ ]"));
                int row_count = tb->rowCount(); //获取表单行数
                tb->insertRow(row_count); //插入新行
                for(int i=0;i<=5;i++){
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setText(newLine.at(i));
                    tb->setItem(row_count,i,newItem);
                }
            }
        }
        file.close();
    }
}

StuInfoTemplate StuSubWindow::getTableForRow(int row)
{
    return StuInfoTemplate(tb->item(row,0)->text(), //学号
                           tb->item(row,1)->text(), //姓名
                           tb->item(row,2)->text(), //性别
                           tb->item(row,3)->text(), //出生日期
                           tb->item(row,4)->text(), //籍贯
                           tb->item(row,5)->text());//住址
}

QString StuSubWindow::getFileNameWithoutFormat(QString filename)
{
    QFileInfo fi(filename);
    QStringList simpleName = fi.fileName().split(QRegExp("[.]"));
    return simpleName.at(0);

}

bool StuSubWindow::primaryKeyConfilct(QString ID,int before)
{
    int curRow = tb->rowCount();
    for(int i=0;i<curRow;i++){
        if(tb->item(i,0)->text() == ID && i!=before){
            QMessageBox::warning(this,tr("Error"),tr("The student of this student number already exists, and cannot be added repeatedly!"));
            return true; //主键chongtu
        }
    }
    return false;
}

void StuSubWindow::closeEvent(QCloseEvent *event)
{
    if(this->flagModified) //文件被修改过了，不能随便关掉
    {
        QMessageBox::StandardButton button;
        QString name = this->windowTitle();
        button=QMessageBox::question(this,tr("Exit the program"),QString(name+tr("Not yet saved, do you want to save?")),QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(button==QMessageBox::No)
        {
            event->accept(); // 不保存，当前窗口直接关闭
        }
        else if(button==QMessageBox::Yes)
        {
            if(this->saveFile()){// 成功保存
                event->accept(); // 接受退出信号，程序退出
            }else{
                event->ignore(); // 忽略退出信号，程序继续进行
            }
        }
        else if(button==QMessageBox::Cancel)
        {
            event->ignore(); // 忽略退出信号，程序继续进行
        }
    }
    else{
        event->accept();
    }

}

void StuSubWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    StuInfoTemplate info = getTableForRow(row);
    info.showStuInfo();
    StuDialog *dialog = new StuDialog();
    dialog->setByStuInfo(info);
    if(dialog->exec() == QDialog::Accepted)
    {
        this->flagModified = true;  //文件被修改了，关闭时需要保存
        QVector<QString> res = dialog->getInput();
        //ID相同，不可重复添加
        QString ID = res[0];
        if(this->primaryKeyConfilct(ID,row)){
            return;
        }
        //ID不存在，可以添加
        for(int i=0;i<=5;i++){
           tb->item(row,i)->setText(res[i]);
        }
    }
}

QString StuSubWindow::getFilePath() const
{
    return filePath;
}

