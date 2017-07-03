#ifndef BUFFER_H
#define BUFFER_H

#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>

class Buffer{ //copyable
public:
    static const size_t kCheapPrepend = 8; // 没有太明确这里的作用
    static const size_t kInitialSize = 1024;

    Buffer() :buffer_(kCheapPrepend + kInitialSize), readerIndex_(kCheapPrepend), writerIndex_(kCheapPrepend){
        assert(readableBytes() == 0);
        assert(writeableBytes() == kInitialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    void swap(Buffer& rhs){
        buffer_.swap(rhs.buffer_); //调用vector swap
        std::swap(readerIndex_, rhs.readerIndex_); //调用std的swap;
        std::swap(writerIndex_, rhs.writerIndex_);
    }

    inline size_t readableBytes() const;
    inline size_t writeableBytes() const;
    inline size_t prependableBytes() const;

    inline const char* peek() const;
    inline void retrieve(size_t len);
    inline void retrieve(const char* end);
    inline void retrieveUntil(const char* end);
    inline void retrieveAll();
    std::string retrieveAsString();
    inline void append(const std::string& str);
    inline void append(const char* data, size_t len);
    inline void append(const void* data, size_t len);
    inline void ensureWriteableBytes(size_t len);
    inline char* beginWrite();
    inline const char* beginWrite() const;
    inline void hasWritten(size_t len);
    inline void prepend(const void* data, size_t len);

    inline void shrink(size_t reserve);

    ssize_t readFd(int fd, int* savedErrno);

private:
    inline char* begin();
    inline const char* begin() const;
    inline void makeSpace(size_t len);

private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};

size_t Buffer::readableBytes() const{
    return writerIndex_ - readerIndex_; //返回可读数量
}

size_t Buffer::writeableBytes() const{
    return buffer_.size() - writerIndex_;
}

size_t Buffer::prependableBytes() const{
    return readerIndex_;
}

const char* Buffer::peek() const{
        return begin() + readerIndex_;
}

void Buffer::retrieve(size_t len){ //len永远小于可读数量
    assert(len <= readableBytes());
    readerIndex_ += len;
}

void Buffer::retrieveUntil(const char* end){//提取到可读前的所有数据
    assert(peek() <= end);
    assert(end <= beginWrite());
    retrieve(end - peek());
}

void Buffer::retrieveAll(){ //??????
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
}

std::string Buffer::retrieveAsString(){
    std::string str(peek(), readableBytes());
    retrieveAll();
    return str;
}

void Buffer::append(const std::string& str){
    append(str.data(), str.length());
}

void Buffer::append(const char* data, size_t len){ //写入len个字符仅buffer
    ensureWriteableBytes(len);
    std::copy(data, data+len, beginWrite());
    hasWritten(len);
}

void Buffer::append(const void* data, size_t len){
    append(static_cast<const char*>(data), len);
}

void Buffer::ensureWriteableBytes(size_t len){ //确保有足够空间写入
    if(writeableBytes() < len){
        makeSpace(len);
    }
    assert(writeableBytes() >= len);
}

char* Buffer::beginWrite(){//返回开始写的位置
    return begin() + writerIndex_;
}

void Buffer::hasWritten(size_t len){ //开始写入
    writerIndex_ += len;
}

void Buffer::prepend(const void* data, size_t len){ //插入在readerIndex前面
    assert(len <= prependableBytes());
    readerIndex_ -= len;
    const char* d = static_cast<const char*>(data);
    std::copy(d, d+len, begin() + readerIndex_);
}

void Buffer::shrink(size_t reserve){
    std::vector<char> buf(kCheapPrepend + readableBytes() +reserve);
    std::copy(peek(), peek() + readerIndex_, buf.begin() + kCheapPrepend);
    buf.swap(buffer_);//将新的空间转换
}




char* Buffer::begin(){
    return &*buffer_.begin();//返回buffer.begin(), 转换成char*
}

const char* Buffer::begin() const{
       // return static_cast<const char*>(begin()); //这样写会导致无线调用吗？
       return &*buffer_.begin();
}

void Buffer::makeSpace(size_t len){
    if(writeableBytes() + prependableBytes() < len + kCheapPrepend){
        buffer_.resize(writerIndex_ +len);
    }
    else{
        assert(kCheapPrepend < readerIndex_);
        size_t readable = readableBytes();
        std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
        readerIndex_ = kCheapPrepend;
        writerIndex_ = readerIndex_ + readable;
        assert(readable == readableBytes());
    }
}

#endif // BUFFER_H
