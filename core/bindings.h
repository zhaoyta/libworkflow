#ifndef __BINDINGS_H_
#define __BINDINGS_H_

#include <tools/defines.h>
#include <tools/jsonable.h>
#include <core/steps.h>


#pragma GCC visibility push(default)
/**
 Meta information as to how inputs are handled from Workflow to first items to execute.
 */
class InputBinding : public Jsonable {
    std::string workflow_input;
    int32_t action_id;
    std::string action_input;
    
public:
    InputBinding(const std::string & workflow_input, int32_t action_id, const std::string & action_input);
    InputBinding();
    virtual ~InputBinding();
    
    int32_t getActionId() const;
    const std::string & getActionInput() const;
    const std::string & getWorkflowInput() const;
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};


/**
 Meta information as to how actions are connected to next actions.
 Might also be used to connect to outside the workflow through the action EStep::Finish
 */
class OutputBinding : public Jsonable {
    int32_t from_action_id;
    std::string from_action_output;
    
    int32_t to_action_id;
    std::string to_action_input;
public:
    OutputBinding();
    OutputBinding(int32_t from_action_id, const std::string & from_action_output,
                  int32_t to_action_id, const std::string & to_action_input);
    OutputBinding(Step from_action_id, const std::string & from_action_output,
                  Step to_action_id, const std::string & to_action_input);
    OutputBinding(const std::string & from_action_output,
                  int32_t to_action_id, const std::string & to_action_input);
    OutputBinding(const std::string & from_action_output,
                  Step to_action_id, const std::string & to_action_input);
    virtual ~OutputBinding();
    
    void setFromActionId(int32_t);
    void setFromActionId(Step);
    
    int32_t getFromActionId() const;
    const std::string & getFromActionOutput() const;
    
    int32_t getToActionId() const;
    const std::string & getToActionInput() const;
    
    void save(boost::property_tree::ptree & root) const override;
    void load(const boost::property_tree::ptree & root) override;
};


#pragma GCC visibility pop

#endif //! __BINDINGS_H_