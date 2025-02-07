#ifndef DISCRETE_PIPELINE_H
#define DISCRETE_PIPELINE_H

#include <functional>
#include <memory>
#include <any>

template <typename InitialInput, typename InitialOutput>
class Pipeline{

    template <
            typename Input,
            typename Output
    >
    struct PipelineStep{

        template <typename NextOutput>
        PipelineStep<Output, NextOutput> step(std::function<NextOutput(Output)> nextStep){
            auto pipelineStep{std::make_unique<PipelineStep<Output, NextOutput>>()};
            pipelineStep->currentStep = nextStep;
            pipelineStep->initial = initial;
            next = pipelineStep;
            return *pipelineStep;
        }

        Pipeline build(){
            Pipeline<InitialInput, InitialOutput> pipeline;
            pipeline.firstStep = initial.get();
        }

        void executeStep(const Input& previousStepResult){
            Output result = currentStep(result);
            if(next){
                executeStep(result);
            }
        }

    private:
        std::function<Output(Input)> currentStep;
        std::unique_ptr<PipelineStep<Output, std::any>> next;
        std::unique_ptr<PipelineStep<InitialInput, InitialOutput>> initial;
    };


    std::unique_ptr<PipelineStep<InitialInput, InitialOutput>> firstStep;

public:

    PipelineStep<InitialInput, InitialOutput> compose(std::function<InitialOutput(InitialInput)> function){
        return PipelineStep<InitialInput, InitialOutput>{}.step(function);
    }

    void operator()(InitialInput input){
        firstStep->executeStep(input);
    }
};

#endif //DISCRETE_PIPELINE_H
