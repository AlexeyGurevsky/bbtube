#ifndef CIPHEROPERATIONS_HPP_
#define CIPHEROPERATIONS_HPP_

class CipherOperation
{
public:
    virtual QByteArray process(QByteArray array) = 0;
    virtual ~CipherOperation()
    {
    }
};

class SpliceCipherOperation: public CipherOperation
{
    int index;
public:
    SpliceCipherOperation(int index) :
            index(index)
    {
    }
    virtual ~SpliceCipherOperation()
    {
    }

    virtual QByteArray process(QByteArray array)
    {
        return array.right(array.size() - index);
    }
};

class ReverseCipherOperation: public CipherOperation
{
public:
    virtual ~ReverseCipherOperation()
    {
    }
    virtual QByteArray process(QByteArray array)
    {
        QByteArray result(array.size(), 0);
        for (int i = 0; i < array.size(); i++) {
            result[i] = array[array.size() - i - 1];
        }

        return result;
    }
};

class SwapCipherOperation: public CipherOperation
{
    int index;

public:
    SwapCipherOperation(int index) :
            index(index)
    {
    }
    virtual ~SwapCipherOperation()
    {
    }

    virtual QByteArray process(QByteArray array)
    {
        QByteArray result = array;
        char c = result[0];
        result[0] = result[index % array.size()];
        result[index % result.size()] = c;

        return result;
    }
};
#endif /* CIPHEROPERATIONS_HPP_ */
