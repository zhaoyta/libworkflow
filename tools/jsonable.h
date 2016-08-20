#ifndef __JSONABLE_H_
#define __JSONABLE_H_

/**
 This class allow other to be serialized to JSON format.
 */
class Jsonable  {
public:
    Jsonable();
    virtual ~Jsonable();
    
    void save(std::string &) const;
    void load(const std::string &);
    
    virtual void save(boost::property_tree::ptree & root) const ;
    virtual void load(const boost::property_tree::ptree & root) ;
    
};


#endif // __JSONABLE_H_