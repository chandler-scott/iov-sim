//
// Generated file, do not edit! Created by opp_msgtool 6.0 from iov_sim/messages/ModelUpdateMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "ModelUpdateMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace iov_sim {

Register_Class(ModelUpdateMessage)

ModelUpdateMessage::ModelUpdateMessage(const char *name, short kind) : ::veins::BaseFrame1609_4(name, kind)
{
}

ModelUpdateMessage::ModelUpdateMessage(const ModelUpdateMessage& other) : ::veins::BaseFrame1609_4(other)
{
    copy(other);
}

ModelUpdateMessage::~ModelUpdateMessage()
{
}

ModelUpdateMessage& ModelUpdateMessage::operator=(const ModelUpdateMessage& other)
{
    if (this == &other) return *this;
    ::veins::BaseFrame1609_4::operator=(other);
    copy(other);
    return *this;
}

void ModelUpdateMessage::copy(const ModelUpdateMessage& other)
{
    this->pStateDict = other.pStateDict;
    this->vStateDict = other.vStateDict;
    this->senderAddress = other.senderAddress;
    this->serial = other.serial;
}

void ModelUpdateMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    doParsimPacking(b,this->pStateDict);
    doParsimPacking(b,this->vStateDict);
    doParsimPacking(b,this->senderAddress);
    doParsimPacking(b,this->serial);
}

void ModelUpdateMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    doParsimUnpacking(b,this->pStateDict);
    doParsimUnpacking(b,this->vStateDict);
    doParsimUnpacking(b,this->senderAddress);
    doParsimUnpacking(b,this->serial);
}

const char * ModelUpdateMessage::getPStateDict() const
{
    return this->pStateDict.c_str();
}

void ModelUpdateMessage::setPStateDict(const char * pStateDict)
{
    this->pStateDict = pStateDict;
}

const char * ModelUpdateMessage::getVStateDict() const
{
    return this->vStateDict.c_str();
}

void ModelUpdateMessage::setVStateDict(const char * vStateDict)
{
    this->vStateDict = vStateDict;
}

const ::veins::LAddress::L2Type& ModelUpdateMessage::getSenderAddress() const
{
    return this->senderAddress;
}

void ModelUpdateMessage::setSenderAddress(const ::veins::LAddress::L2Type& senderAddress)
{
    this->senderAddress = senderAddress;
}

int ModelUpdateMessage::getSerial() const
{
    return this->serial;
}

void ModelUpdateMessage::setSerial(int serial)
{
    this->serial = serial;
}

class ModelUpdateMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_pStateDict,
        FIELD_vStateDict,
        FIELD_senderAddress,
        FIELD_serial,
    };
  public:
    ModelUpdateMessageDescriptor();
    virtual ~ModelUpdateMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ModelUpdateMessageDescriptor)

ModelUpdateMessageDescriptor::ModelUpdateMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(iov_sim::ModelUpdateMessage)), "veins::BaseFrame1609_4")
{
    propertyNames = nullptr;
}

ModelUpdateMessageDescriptor::~ModelUpdateMessageDescriptor()
{
    delete[] propertyNames;
}

bool ModelUpdateMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ModelUpdateMessage *>(obj)!=nullptr;
}

const char **ModelUpdateMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ModelUpdateMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ModelUpdateMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int ModelUpdateMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_pStateDict
        FD_ISEDITABLE,    // FIELD_vStateDict
        0,    // FIELD_senderAddress
        FD_ISEDITABLE,    // FIELD_serial
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *ModelUpdateMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "pStateDict",
        "vStateDict",
        "senderAddress",
        "serial",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int ModelUpdateMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "pStateDict") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "vStateDict") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "senderAddress") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "serial") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *ModelUpdateMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_pStateDict
        "string",    // FIELD_vStateDict
        "veins::LAddress::L2Type",    // FIELD_senderAddress
        "int",    // FIELD_serial
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **ModelUpdateMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ModelUpdateMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ModelUpdateMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ModelUpdateMessage *pp = omnetpp::fromAnyPtr<ModelUpdateMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ModelUpdateMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ModelUpdateMessage *pp = omnetpp::fromAnyPtr<ModelUpdateMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ModelUpdateMessage'", field);
    }
}

const char *ModelUpdateMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ModelUpdateMessage *pp = omnetpp::fromAnyPtr<ModelUpdateMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ModelUpdateMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ModelUpdateMessage *pp = omnetpp::fromAnyPtr<ModelUpdateMessage>(object); (void)pp;
    switch (field) {
        case FIELD_pStateDict: return oppstring2string(pp->getPStateDict());
        case FIELD_vStateDict: return oppstring2string(pp->getVStateDict());
        case FIELD_senderAddress: return "";
        case FIELD_serial: return long2string(pp->getSerial());
        default: return "";
    }
}

void ModelUpdateMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ModelUpdateMessage *pp = omnetpp::fromAnyPtr<ModelUpdateMessage>(object); (void)pp;
    switch (field) {
        case FIELD_pStateDict: pp->setPStateDict((value)); break;
        case FIELD_vStateDict: pp->setVStateDict((value)); break;
        case FIELD_serial: pp->setSerial(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ModelUpdateMessage'", field);
    }
}

omnetpp::cValue ModelUpdateMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ModelUpdateMessage *pp = omnetpp::fromAnyPtr<ModelUpdateMessage>(object); (void)pp;
    switch (field) {
        case FIELD_pStateDict: return pp->getPStateDict();
        case FIELD_vStateDict: return pp->getVStateDict();
        case FIELD_senderAddress: return omnetpp::toAnyPtr(&pp->getSenderAddress()); break;
        case FIELD_serial: return pp->getSerial();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ModelUpdateMessage' as cValue -- field index out of range?", field);
    }
}

void ModelUpdateMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ModelUpdateMessage *pp = omnetpp::fromAnyPtr<ModelUpdateMessage>(object); (void)pp;
    switch (field) {
        case FIELD_pStateDict: pp->setPStateDict(value.stringValue()); break;
        case FIELD_vStateDict: pp->setVStateDict(value.stringValue()); break;
        case FIELD_serial: pp->setSerial(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ModelUpdateMessage'", field);
    }
}

const char *ModelUpdateMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ModelUpdateMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ModelUpdateMessage *pp = omnetpp::fromAnyPtr<ModelUpdateMessage>(object); (void)pp;
    switch (field) {
        case FIELD_senderAddress: return omnetpp::toAnyPtr(&pp->getSenderAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ModelUpdateMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ModelUpdateMessage *pp = omnetpp::fromAnyPtr<ModelUpdateMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ModelUpdateMessage'", field);
    }
}

}  // namespace iov_sim

namespace omnetpp {

}  // namespace omnetpp

