#pragma once
#include "references.hpp"
#include "segment.hpp"
#include <memory>

namespace persistence
{
    struct WriterReferenceContext
    {
        ReferenceTable<std::shared_ptr<void>, Segments> references;
    };

    template<typename GlobalContext>
    struct OutputContext
    {
        OutputContext(GlobalContext& global)
            : global_context(global)
            , parent_context(nullptr)
        {}

        OutputContext(OutputContext<GlobalContext>& local, Segment segment)
            : global_context(local.global_context)
            , parent_context(&local)
            , segment(segment)
        {}

        Segments get(std::shared_ptr<void> pointer) const
        {
            return global_context.references.get(pointer);
        }

        void put(std::shared_ptr<void> pointer)
        {
            global_context.references.put(pointer, segments());
        }

        GlobalContext& global()
        {
            return global_context;
        }

        std::vector<Segment> segments() const
        {
            std::vector<Segment> segs;
            const OutputContext<GlobalContext>* ctx = this;
            while (ctx->parent_context != nullptr) {
                segs.push_back(ctx->segment);
                ctx = ctx->parent_context;
            };
            return segs;
        }

    private:
        GlobalContext& global_context;
        OutputContext<GlobalContext>* parent_context;
        Segment segment;
    };
}
