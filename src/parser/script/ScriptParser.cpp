#include "ScriptParser.hpp"
#include "ScriptData.hpp"
#include "src/parser/cipher/CipherOperations.hpp"

#include <QRegExp>
#include <QStringList>

ScriptData ScriptParser::parse(QString script)
{
    QRegExp cipherFunctionNameRegExp("[a-zA-Z]*\\(decodeURIComponent\\([a-z]\\)\\),[a-z]\\.set");
    cipherFunctionNameRegExp.indexIn(script);
    QString cipherFunctionCall = cipherFunctionNameRegExp.capturedTexts()[0];
    QString cipherFunctionName = cipherFunctionCall.left(cipherFunctionCall.indexOf('('));

    QRegExp cipherFunctionTextRegExp(cipherFunctionName + "=function\\([a-z]\\)\\{.*\\}");
    cipherFunctionTextRegExp.setMinimal(true);
    cipherFunctionTextRegExp.indexIn(script);
    QString cipherFunctionText = cipherFunctionTextRegExp.capturedTexts()[0];
    QStringList cipherAlgorithmsList;
    QRegExp cipherAlgorithmsListRegExp("[a-zA-Z0-9]{2}\\.[a-zA-Z0-9]{2}(\\([a-z],\\d*\\)|\\([a-z]\\))");
    int pos = 0;
    while (pos >= 0) {
        pos = cipherAlgorithmsListRegExp.indexIn(cipherFunctionText, pos);
        if (pos >= 0) {
            cipherAlgorithmsList.append(cipherAlgorithmsListRegExp.cap(0));
            pos += cipherAlgorithmsListRegExp.matchedLength();
        }
    }

    QString cipherObjectName = cipherAlgorithmsList[0].left(cipherAlgorithmsList[0].indexOf('.'));
    QRegExp cipherObjectDefinitionRegExp("var " + cipherObjectName + "=\\{(.|\\n)*\\};");
    cipherObjectDefinitionRegExp.setMinimal(true);
    cipherObjectDefinitionRegExp.indexIn(script);
    QString cipherObjectDefinition = cipherObjectDefinitionRegExp.capturedTexts()[0].replace("\n",
            "");

    QList<CipherOperation*> cipherOperations;
    for (int i = 0; i < cipherAlgorithmsList.size(); i++) {
        QString functionName = cipherAlgorithmsList[i].mid(cipherAlgorithmsList[i].indexOf('.') + 1,
                cipherAlgorithmsList[i].indexOf('(') - cipherAlgorithmsList[i].indexOf('.') - 1);
        QRegExp functionBodyRegExp(functionName + ":function\\(.*\\)\\{.*\\}");
        functionBodyRegExp.setMinimal(true);
        functionBodyRegExp.indexIn(cipherObjectDefinition);
        QString functionBody = functionBodyRegExp.capturedTexts()[0];

        if (functionBody.indexOf("reverse") >= 0) {
            cipherOperations.append(new ReverseCipherOperation());
        } else if (functionBody.indexOf("splice") >= 0) {
            int index =
                    cipherAlgorithmsList[i].mid(cipherAlgorithmsList[i].indexOf(',') + 1,
                            cipherAlgorithmsList[i].indexOf(')')
                                    - cipherAlgorithmsList[i].indexOf(',') - 1).toInt();
            cipherOperations.append(new SpliceCipherOperation(index));
        } else {
            int index =
                    cipherAlgorithmsList[i].mid(cipherAlgorithmsList[i].indexOf(',') + 1,
                            cipherAlgorithmsList[i].indexOf(')')
                                    - cipherAlgorithmsList[i].indexOf(',') - 1).toInt();
            cipherOperations.append(new SwapCipherOperation(index));
        }
    }

    ScriptData scriptData;
    scriptData.cipherOperations = cipherOperations;

    return scriptData;
}

