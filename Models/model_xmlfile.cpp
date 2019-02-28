#include "model_xmlfile.h"

Model_XMLFile::Model_XMLFile()
{

}

/*************************************************************
/函数功能：序列XML--创建（每次均新建，将删除之前数据）
/函数参数：文件路径
/函数返回：无
*************************************************************/
void Model_XMLFile::createSequenceXML(QString filePath)
{
    QFile file(filePath); //关联文件名字

        //此为刷新文件，覆盖
        if(true == file.open(QIODevice::WriteOnly))
        {
            //创建xml文档对象
            QDomDocument doc;
            //创建xml头部格式 <?xml version='1.0' encoding='utf-8'?>
            QDomProcessingInstruction ins = doc.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'utf-8\'");

            //追加元素
            doc.appendChild(ins);

            //根节点元素
            QDomElement root = doc.createElement("Sequence");
            doc.appendChild(root);

            QDomText text = doc.createTextNode("");
            root.appendChild(text);

            //保存
            QTextStream stream(&file); //文本流关联文件
            doc.save(stream, 4); //4 缩进字符

            file.close(); //关闭文件
        }
        else
        {
            cout << "WriteOnly error";
            return;
        }

}

/*************************************************************
/函数功能：按键XML--查询是否存在项目
/函数参数：item
/函数返回：结果
*************************************************************/
bool Model_XMLFile::hasUnitInfomation(QString filePath,QString unitName)
{
    QFile file(filePath);
    bool isOk = file.open(QIODevice::ReadOnly);
    if(true == isOk) //打开成功
    {
        //file和xml文档对象关联
        QDomDocument doc;
        isOk = doc.setContent(&file);
        if(isOk) //如果关联成功
        {
            file.close(); //关闭文件
            //获取根节点元素
            QDomElement root = doc.documentElement();

            //判断根节点下有没有子节点
            if(root.hasChildNodes())
            {
                QDomNode n = root.firstChild();

                while(!n.isNull())
                {
                    QDomElement thisEmt = n.toElement();
                    //cout << thisEmt.attribute("name");

                    if( (!thisEmt.isNull()) && (thisEmt.attribute("name") == unitName) )
                    {
                        //已存在该项目，修改数据
                        //cout << unitName;
                        return true;
                    }
                    n=n.nextSibling();
                }
            }
        }
    }

    return false;
}

/*************************************************************
/函数功能：序列XML--添加序列
/函数参数：文件路径    测试单元   动作
/函数返回：无
*************************************************************/
void Model_XMLFile::appendSequenceXML(QString filePath,QStringList listUnit,tAction list)
{
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) //打开成功
    {
        //file和xml文档对象关联
        QDomDocument doc;
        if(doc.setContent(&file)) //如果关联成功
        {
            file.close(); //关闭文件
            //获取根节点元素
            QDomElement root = doc.documentElement();

            //判断根节点下有没有子节点
            if( root.hasChildNodes() ) //如果有子节点
            {
                //查找最后一个子节点
                QDomElement lastEmt = root.lastChildElement();

                if((lastEmt.attribute("name") == listUnit.at(0))&&(lastEmt.attribute("cycle") == listUnit.at(1)))
                    writeSequenceActXML(doc, lastEmt, list);        //有当前属性节点，写有效数据
                else
                    writeSequenceUnitXML(doc,root,listUnit,list);   //如果没有，添加测试单元节点
            }
            else
                writeSequenceUnitXML(doc,root,listUnit,list);       //没有子节点，添加测试单元节点

            //保存文件
            if(file.open(QIODevice::WriteOnly))
            {
                QTextStream stream(&file);
                doc.save(stream, 4);
                file.close();
            }
        }
        else
        {
            cout << "setContent error";
            file.close(); //关闭文件
        }
    }
    else
        cout << "ReadOnly error";
}

/*************************************************************
/函数功能：序列XML--添加测试单元
/函数参数：dom节点   根节点  测试单元  动作
/函数返回：无
*************************************************************/
void Model_XMLFile::writeSequenceUnitXML(QDomDocument &doc, QDomElement &root, QStringList &unitList,tAction &list)
{
    if(unitList.length()<UnitInfo_n)
    {
        return ;
    }

    //创建测试单元子节点元素
    QDomElement unitEmt = doc.createElement("Unit");

    //创建name属性
    QDomAttr nameAttr = doc.createAttribute("name");
    nameAttr.setNodeValue(unitList.at(0));
    //创建cycle属性
    QDomAttr cycleAttr = doc.createAttribute("cycle");
    cycleAttr.setNodeValue(unitList.at(1));

    //创建描述节点元素
    QDomElement desEmt = doc.createElement("描述");
    QDomText text = doc.createTextNode(unitList.at(2));
    desEmt.appendChild(text);


    //节点和属性关联
    unitEmt.setAttributeNode(nameAttr);
    //节点和属性关联
    unitEmt.setAttributeNode(cycleAttr);


    //把描述添加到测试单元子节点
    unitEmt.appendChild(desEmt);
    //把测试单元节点追加到根节点上
    root.appendChild(unitEmt);

    //写有效数据
    writeSequenceActXML(doc, unitEmt, list);
}

/*************************************************************
/函数功能：序列XML--添加动作
/函数参数：dom:节点  root：动作添加的节点  动作
/函数返回：无
*************************************************************/
void Model_XMLFile::writeSequenceActXML(QDomDocument &doc, QDomElement &root, tAction &list)
{
    //创建时间节点元素
    QDomElement actEmt = doc.createElement("动作");

    //动作节点添加到根节点
    root.appendChild(actEmt);

    //添加名字：actName
    QDomElement nameEle = doc.createElement("Name");
    nameEle.appendChild(doc.createTextNode(list.actName));
    actEmt.appendChild(nameEle);

    //添加动作处理：actDeal
    QDomElement stringEle = doc.createElement("String");
    stringEle.appendChild(doc.createTextNode(list.actStr));
    actEmt.appendChild(stringEle);

    //添加act类型标志位：actFlag
    QDomElement actflagEle = doc.createElement("actFlag");
    actflagEle.appendChild(doc.createTextNode(toStr(list.actFlag)));
    actEmt.appendChild(actflagEle);

    //添加信息采集标志位：infoFlag
    QDomElement infoEle = doc.createElement("InfoFlag");
    infoEle.appendChild(doc.createTextNode(toStr(list.infoFlag)));
    actEmt.appendChild(infoEle);

    //时间处理
    QDomElement timeEle = doc.createElement("Time");
    actEmt.appendChild(timeEle);

    QDomElement waitEle = doc.createElement("Wait");
    waitEle.appendChild(doc.createTextNode(toStr(list.timeDeal.wait)));
    timeEle.appendChild(waitEle);

    QDomElement checkEle = doc.createElement("Check");
    checkEle.appendChild(doc.createTextNode(toStr(list.timeDeal.check)));
    timeEle.appendChild(checkEle);

    QDomElement endEle = doc.createElement("End");
    endEle.appendChild(doc.createTextNode(toStr(list.timeDeal.end)));
    timeEle.appendChild(endEle);

    //check处理：
    if(list.checkDeal.isEmpty()==false)
    {
        QDomElement checkEle = doc.createElement("Check");
        actEmt.appendChild(checkEle);

        for(int i=0;i<list.checkDeal.length();i++)
        {
            checkParam chk = list.checkDeal.at(i);

            QDomElement chktypeEle = doc.createElement("ChkType");
            chktypeEle.appendChild(doc.createTextNode(toStr(chk.check)));
            checkEle.appendChild(chktypeEle);

            switch(chk.check)
            {
            case CHKCurrent:
            case CHKVlot:
                {
                    QDomElement rangeEle = doc.createElement("Range");
                    rangeEle.appendChild(doc.createTextNode(toStr(chk.range)));
                    checkEle.appendChild(rangeEle);

                    QDomElement minEle = doc.createElement("Min");
                    minEle.appendChild(doc.createTextNode(toStr(chk.min)));
                    checkEle.appendChild(minEle);

                    if(chk.range == GELE)
                    {
                        QDomElement maxEle = doc.createElement("Max");
                        maxEle.appendChild(doc.createTextNode(toStr(chk.max)));
                        checkEle.appendChild(maxEle);
                    }
                }
                break;

            case CHKSound:
                {
                    QDomElement soundEle = doc.createElement("Sound");
                    soundEle.appendChild(doc.createTextNode(toStr(chk.sound)));
                    checkEle.appendChild(soundEle);
                }
                break;

            case CHKScript:
                {
                    QDomElement scriptEle = doc.createElement("Script");
                    scriptEle.appendChild(doc.createTextNode(chk.logContains));
                    checkEle.appendChild(scriptEle);
                }
                break;

            case CHKInterface:
            case CHKADBPIC:
            {
                QDomElement memEle = doc.createElement("InfoCompare");
                memEle.appendChild(doc.createTextNode(toStr(chk.infoCompare)));
                checkEle.appendChild(memEle);
                break;
            }
            default:
                break;
            }
        }
    }

    //changed处理：
    if(list.changedDeal.isEmpty()==false)
    {
        QDomElement changedEle = doc.createElement("Changed");
        actEmt.appendChild(changedEle);

        for(int i=0;i<list.changedDeal.length();i++)
        {
            changedParam cng = list.changedDeal.at(i);

            QDomElement cngtypeEle = doc.createElement("ChangedType");
            cngtypeEle.appendChild(doc.createTextNode(toStr(cng.changed)));
            changedEle.appendChild(cngtypeEle);

            QDomElement dirEle = doc.createElement("Dir");
            dirEle.appendChild(doc.createTextNode(toStr(cng.dir)));
            changedEle.appendChild(dirEle);
            QDomElement minEle = doc.createElement("Min");
            minEle.appendChild(doc.createTextNode(toStr(cng.min)));
            changedEle.appendChild(minEle);
            QDomElement maxEle = doc.createElement("Max");
            maxEle.appendChild(doc.createTextNode(toStr(cng.max)));
            changedEle.appendChild(maxEle);
            QDomElement stepEle = doc.createElement("Step");
            stepEle.appendChild(doc.createTextNode(toStr(cng.step)));
            changedEle.appendChild(stepEle);
        }
    }
}

/*************************************************************
/函数功能：序列XML--读取测试流
/函数参数：文件路径  测试流
/函数返回：无
*************************************************************/
void Model_XMLFile::readSequenceXML(QString filePath,QList <tUnit> &tFlow)
{
    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly))
        return ;

    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        return ;
    }
    file.close();

    //获取根节点
    QDomElement root = doc.documentElement();

    if(!root.hasChildNodes())
        return ;

    QDomNode n = root.firstChild();

    while(!n.isNull())
    {
        QDomElement thisEmt = n.toElement();

        if(!thisEmt.isNull())
        {
            tUnit unit;

            unit.name = thisEmt.attribute("name");
            unit.cycleCount = thisEmt.attribute("cycle").toUInt();
            QDomNodeList list = thisEmt.childNodes();

            if(list.isEmpty()==false)
            {
                for(int i=0;i<list.length();i++)
                {
                    QDomNode unitNode = list.at(i);
                    QString nodeName = unitNode.toElement().nodeName();

                    if(nodeName == "描述")
                        unit.unitDes = unitNode.toElement().text();
                    else if(nodeName == "动作")
                    {
                        //后续动作均进入此函数
                        QDomNodeList actEmt = unitNode.childNodes();
                        tAction act;

                        if(actEmt.isEmpty() == false)
                        {
                            for(int j=0;j<actEmt.length();j++)
                            {
                                QDomNode actEle = actEmt.at(j);
                                QString emtName = actEle.toElement().nodeName();

                                if(emtName == "Name")
                                    act.actName = actEle.toElement().text();
                                else if(emtName == "String")
                                    act.actStr = actEle.toElement().text();
                                else if(emtName == "actFlag")
                                    act.actFlag = actEle.toElement().text().toUInt();
                                else if(emtName == "InfoFlag")
                                    act.infoFlag = actEle.toElement().text().toUInt();
                                else if(emtName == "Time")
                                {
                                    QDomNodeList timelist = actEle.childNodes();

                                    if(timelist.length() ==3)
                                    {
                                        act.timeDeal.wait = timelist.at(0).toElement().text().toUInt();
                                        act.timeDeal.check = timelist.at(1).toElement().text().toUInt();
                                        act.timeDeal.end = timelist.at(2).toElement().text().toUInt();
                                    }
                                    else
                                        cout << "XMl 存储的序列时间有误";
                                }
                                else if(emtName == "Check")
                                {
                                    QDomNodeList checklist = actEle.childNodes();
                                    checkParam chkDat;
                                    initNullChkParam(&chkDat);

                                    for(int c=0;c<checklist.length();c++)
                                    {
                                        QDomNode chkNode = checklist.at(c);
                                        QString mleName = chkNode.toElement().nodeName();
                                        if(mleName == "ChkType")
                                        {
                                            //非起始数据，且再次回到检测类型时
                                            if(c)
                                                act.checkDeal.append(chkDat);
                                            chkDat.check = (chk_type_e)chkNode.toElement().text().toUInt();
                                        }
                                        else if(mleName == "Range")
                                            chkDat.range = (range_type_e)chkNode.toElement().text().toUInt();
                                        else if(mleName == "Min")
                                            chkDat.min = chkNode.toElement().text().toUInt();
                                        else if(mleName == "Max")
                                            chkDat.max = chkNode.toElement().text().toUInt();
                                        else if(mleName == "Sound")
                                            chkDat.sound = (sound_type_e)chkNode.toElement().text().toUInt();
                                        else if(mleName == "Script")
                                            chkDat.logContains = chkNode.toElement().text();
                                        else if(mleName == "InfoCompare")
                                            chkDat.infoCompare = (compare_type_e)chkNode.toElement().text().toUInt();
                                    }
                                    act.checkDeal.append(chkDat);//最后一次数据保存
                                }
                                else if(emtName == "Changed")
                                {
                                    QDomNodeList changedlist = actEle.childNodes();
                                    changedParam cngDat;

                                    for(int c=0;c<changedlist.length();c++)
                                    {
                                        QDomNode cngNode = changedlist.at(c);
                                        QString mleName = cngNode.toElement().nodeName();
                                        if(mleName == "ChangedType")
                                        {
                                            //非起始数据，且再次回到检测类型时
                                            if(c)
                                                act.changedDeal.append(cngDat);
                                            cngDat.changed = (changed_type_e)cngNode.toElement().text().toUInt();
                                        }
                                        else if(mleName == "Dir")
                                            cngDat.dir = cngNode.toElement().text().toUInt();
                                        else if(mleName == "Min")
                                            cngDat.min = cngNode.toElement().text().toUInt();
                                        else if(mleName == "Max")
                                            cngDat.max = cngNode.toElement().text().toUInt();
                                        else if(mleName == "Step")
                                            cngDat.step = cngNode.toElement().text().toUInt();
                                    }
                                    act.changedDeal.append(cngDat);//最后一次数据保存
                                }
                            }
                            unit.actTest.append(act);
                        }
                    }
                }
            }
            tFlow.append(unit);
        }
        n=n.nextSibling();
    }
}

/*************************************************************
/函数功能：移除测试单元
/函数参数：文件路径  测试流
/函数返回：无
*************************************************************/
void Model_XMLFile::removeUnitXML(QString filePath,QString unitName)
{
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) //打开成功
    {
        //file和xml文档对象关联
        QDomDocument doc;
        if(doc.setContent(&file)) //如果关联成功
        {
            file.close(); //关闭文件
            //获取根节点元素
            QDomElement root = doc.documentElement();

            //判断根节点下有没有子节点
            if(root.hasChildNodes())
            {
                QDomNode n = root.firstChild();

                while(!n.isNull())
                {
                    QDomElement thisEmt = n.toElement();

                    if( (!thisEmt.isNull()) && (thisEmt.attribute("name") == unitName) )
                    {
                        root.removeChild(n);
                        break;
                    }
                    n=n.nextSibling();
                }
            }

            //保存文件
            if(file.open(QIODevice::WriteOnly))
            {
                QTextStream stream(&file);
                doc.save(stream, 4);
                file.close();
            }
        }
        else
            file.close(); //关闭文件
    }
}



/*************************************************************
/函数功能：按键XML--创建文件，若存在跳过
/函数参数：无
/函数返回：无
*************************************************************/
void Model_XMLFile::createKeyInfoXML()
{
    QFile file(KeyINFOFile); //关联文件名字
    if( true == file.exists() ) //如果存在不创建
    {
        cout << "文件已经存在";
        return;
    }
    else
    { //不存在才创建
        //只写方式打开文件
        bool isOk = file.open(QIODevice::WriteOnly);
        if(true == isOk)//如果打开成功
        {
            //创建xml文档对象
            QDomDocument doc;
            //创建xml头部格式 <?xml version='1.0' encoding='utf-8'?>
            QDomProcessingInstruction ins = doc.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'utf-8\'");

            //追加元素
            doc.appendChild(ins);

            //根节点元素
            QDomElement root = doc.createElement("KeyInformation");
            doc.appendChild(root);

            QDomText text = doc.createTextNode("");
            root.appendChild(text);

            //保存
            QTextStream stream(&file); //文本流关联文件
            doc.save(stream, 4); //4 缩进字符

            file.close(); //关闭文件
        }
        else
        {
            cout << "WriteOnly error";
            return;
        }
    }
    //qDebug()<<__FUNCTION__;
}

/*************************************************************
/函数功能：按键XML--查询是否存在项目
/函数参数：item
/函数返回：结果
*************************************************************/
bool Model_XMLFile::hasItemKeyInfomation(QString item)
{
    QFile file(KeyINFOFile);
    bool isOk = file.open(QIODevice::ReadOnly);
    if(true == isOk) //打开成功
    {
        //file和xml文档对象关联
        QDomDocument doc;
        isOk = doc.setContent(&file);
        if(isOk) //如果关联成功
        {
            file.close(); //关闭文件
            //获取根节点元素
            QDomElement root = doc.documentElement();

            //判断根节点下有没有子节点
            if( !root.hasChildNodes() ) //如果有子节点
            {
                return false;
            }
            else
            {
                QDomNode n = root.firstChild();

                while(!n.isNull())
                {
                    QDomElement thisEmt = n.toElement();

                    if( (!thisEmt.isNull()) && (thisEmt.attribute("name") == item) )
                    {
                        if(thisEmt.lastChildElement().attribute("keyTab")==("KEY"+QString::number(MaxKey)))
                        {
                            //已存在该项目，修改数据
                            return true;
                        }
                    }
                    n=n.nextSibling();
                }
            }
        }
        else
        {
            cout << "setContent error";
        }
    }
    else
    {
        cout << "ReadOnly error";
    }

    return false;
}

/*************************************************************
/函数功能：按键XML--添加项目信息
/函数参数：项目 是否添加  list
/函数返回：无
*************************************************************/
void Model_XMLFile::appendKeyInfoXML(QString item,bool isAppend,QStringList list)
{
    QFile file(KeyINFOFile);
    if(!file.open(QIODevice::ReadOnly))
    {
        cout << "ReadOnly error";
        return ;
    }

    //file和xml文档对象关联
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        cout << "setContent error";
        return ;
    }

    file.close(); //关闭文件

    //获取根节点元素
    QDomElement root = doc.documentElement();

    //判断根节点下有没有子节点
    if( root.hasChildNodes() ) //如果有子节点
    {
        QDomNode n = root.firstChild();

        while(!n.isNull())
        {
            QDomElement thisEmt = n.toElement();

            if( (!thisEmt.isNull()) && (thisEmt.attribute("name") == item) )
            {
                if(!isAppend)
                {
                    //修改数据
                    //cout<<"修改"+item;
                    changedKeyInfoXML(doc,thisEmt,list);
                }
                else
                {
                    //添加数据
                    writeKeyInfoXML(doc, thisEmt, list);
                    //cout<<"new key"<<list.at(0);
                }
                break ;
            }
            else if(thisEmt==root.lastChildElement())
            {
                //添加测试单元节点
                writeKeyInfoItemXML(doc,root,item,list);
                //cout<<"new node last";
                break;
            }
            n=n.nextSibling();
        }
    }
    //没有子节点
    else
    {
        //添加测试单元节点
        writeKeyInfoItemXML(doc,root,item,list);
        //cout<<"new node";
    }

    //保存文件
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        doc.save(stream, 4);
        file.close();
    }
}

/*************************************************************
/函数功能：按键XML--添加项目信息
/函数参数：
/函数返回：无
*************************************************************/
void Model_XMLFile::writeKeyInfoItemXML(QDomDocument &doc, QDomElement &root, QString &item,QStringList &list)
{
    //创建测试单元子节点元素
    QDomElement unitEmt = doc.createElement("ITEM");


    //创建name属性
    QDomAttr nameAttr = doc.createAttribute("name");
    nameAttr.setNodeValue(item);
    //节点和属性关联
    unitEmt.setAttributeNode(nameAttr);
    //把测试单元节点追加到根节点上
    root.appendChild(unitEmt);

    //写有效数据
    writeKeyInfoXML(doc, unitEmt, list);
}

/*************************************************************
/函数功能：按键XML--添加按键信息
/函数参数：
/函数返回：无
*************************************************************/
void Model_XMLFile::writeKeyInfoXML(QDomDocument &doc, QDomElement &root, QStringList &list)
{
    if(list.length()<KeyInfo_n)
    {
        cout << "error:list :"<<list.length()<<" is over.";
        return ;
    }
    //创建时间节点元素
    QDomElement actEmt = doc.createElement("KEY");

    //创建按键标号属性：为了同一个项目可根据按键号修改相应内容
    QDomAttr tabAttr = doc.createAttribute("keyTab");
    tabAttr.setNodeValue(list.at(0));
    //节点和属性关联
    actEmt.setAttributeNode(tabAttr);

    //把时间节点追击到日期节点后面
    root.appendChild(actEmt);


    QDomElement name = doc.createElement("name");
    QDomElement isUse = doc.createElement("isUse");
    QDomElement des = doc.createElement("description");
    QDomElement type = doc.createElement("type");
    QDomElement CANID = doc.createElement("CANID");
    QDomElement CANDat1 = doc.createElement("CANDat1");
    QDomElement CANDat2 = doc.createElement("CANDat2");


    QDomText text = doc.createTextNode(list.at(1));
    name.appendChild(text);

    text = doc.createTextNode(list.at(2));
    isUse.appendChild(text);

    text = doc.createTextNode(list.at(3));
    des.appendChild(text);

    text = doc.createTextNode(list.at(4));
    type.appendChild(text);

    text = doc.createTextNode(list.at(5));
    CANID.appendChild(text);

    text = doc.createTextNode(list.at(6));
    CANDat1.appendChild(text);

    text = doc.createTextNode(list.at(7));
    CANDat2.appendChild(text);


    actEmt.appendChild(name);
    actEmt.appendChild(isUse);
    actEmt.appendChild(des);
    actEmt.appendChild(type);
    actEmt.appendChild(CANID);
    actEmt.appendChild(CANDat1);
    actEmt.appendChild(CANDat2);

}

/*************************************************************
/函数功能：按键XML--修改按键信息
/函数参数：
/函数返回：无
/数据量较大，单个修改太麻烦，建议删除子节点在进行添加
*************************************************************/
void Model_XMLFile::changedKeyInfoXML(QDomDocument &doc, QDomElement &root, QStringList &list)
{
    QDomNode node = root.firstChild();
    while(!node.isNull())
    {
        QDomElement keyElem = node.toElement();
        if((!keyElem.isNull()) && (keyElem.attribute("keyTab")==list.at(0)))
        {
            root.removeChild(node);
            writeKeyInfoXML(doc, root, list);
            break;
        }
        node = node.nextSibling();
    }
}

/*************************************************************
/函数功能：按键XML--读取按键信息
/函数参数：
/函数返回：无
*************************************************************/
void Model_XMLFile::readKeyInfoXML(QString item, QList <keyControl> *keyList)
{
    QFile file(KeyINFOFile);

    if(!file.open(QIODevice::ReadOnly))
        return ;

    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        return ;
    }
    file.close();

    //获取根节点
    QDomElement root = doc.documentElement();

    if(!root.hasChildNodes())
        return ;

    QDomNode n = root.firstChild();

    while(!n.isNull())
    {
        QDomElement thisEmt = n.toElement();

        if( (!thisEmt.isNull()) && (item == thisEmt.attribute("name")) )
        {
            QDomNodeList list = thisEmt.childNodes();

            for(int i=0;i<list.length();i++)
            {
                QDomNodeList keyEmt = list.at(i).childNodes();

                if(keyEmt.length()>=(KeyInfo_n-1))
                {
                    keyControl keyInfo;

                    keyInfo.name    =   keyEmt.at(0).toElement().text();
                    keyInfo.isUse   =   keyEmt.at(1).toElement().text().toUInt();
                    keyInfo.des     =   keyEmt.at(2).toElement().text();
                    keyInfo.type    =   (kType) keyEmt.at(3).toElement().text().toUInt();
                    keyInfo.CANID   =   keyEmt.at(4).toElement().text();
                    keyInfo.CANDat1 =   keyEmt.at(5).toElement().text();
                    keyInfo.CANDat2 =   keyEmt.at(6).toElement().text();
                    keyList->append(keyInfo);
                }
                //cout << item << keyInfo.name;
            }
        }
        n=n.nextSibling();
    }
}

/*************************************************************
/函数功能：按键XML--读取项目信息
/函数参数：
/函数返回：无
*************************************************************/
void Model_XMLFile::readKeyItemXML(QStringList *itemList)
{
    QFile file(KeyINFOFile);

    if(!file.open(QIODevice::ReadOnly))
        return ;

    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        return ;
    }
    file.close();

    //获取根节点
    QDomElement root = doc.documentElement();

    if(!root.hasChildNodes())
        return ;

    QDomNode n = root.firstChild();

    while(!n.isNull())
    {
        QDomElement thisEmt = n.toElement();

        if(!thisEmt.isNull())
        {
            itemList->append(thisEmt.attribute("name"));
        }
        n=n.nextSibling();
    }
}

