
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <utility>

// Include paths
#include "DocumentBuilder/Document.h"
#include "WorkflowInstance/DocCompletionCheckNode.h"
#include "WorkflowInstance/RangeCheckNode.h"
#include "WorkflowInstance/WorkflowDefinition.h"
#include "WorkflowInstance/WorkflowInstance.h"

static std::string str(HandleResult r) {
    switch (r) {
        case HandleResult::PASSED:    return "PASSED";
        case HandleResult::REJECTED:  return "REJECTED";
        case HandleResult::PENDING:   return "PENDING";
        case HandleResult::FAILED:    return "FAILED";
        case HandleResult::CANCELLED: return "CANCELLED";
    }
    return "UNKNOWN";
}

static std::shared_ptr<Document> makeDoc(
    std::map<std::string, std::string> common,
    std::map<std::string, double> comparable)
{
    std::shared_ptr<Document> d(new Document());

    d->setDocument(std::move(common), std::move(comparable));

    return d;
}

static std::vector<std::shared_ptr<Comparable> > standardRules() {
    std::vector<std::shared_ptr<Comparable> > rules;

    rules.push_back(std::shared_ptr<Comparable>(
        new Comparable("age", 18.0, ">=")));

    rules.push_back(std::shared_ptr<Comparable>(
        new Comparable("salary", 30000.0, ">=")));

    rules.push_back(std::shared_ptr<Comparable>(
        new Comparable("score", 70.0, ">=")));

    return rules;
}

// Temporary manual chain traversal.
// This does not test automatic chaining inside handle().
static void runChainManually(WorkflowNode* node, WorkflowInstance& wi) {
    while (node) {
        HandleResult r = node->handle(wi);

        std::cout << "    " << node->getName()
                  << " -> " << str(r) << "\n";

        if (r != HandleResult::PASSED) {
            return;
        }

        node = node->getNext();
    }
}

int main() {
    std::cout << "===== Workflow smoke test =====\n";

    // 1. Document construction
    std::shared_ptr<Document> doc = makeDoc(
        {
            {"name", "John Doe"},
            {"address", "123 Main St"},
            {"email", "john@example.com"}
        },
        {
            {"age", 30.0},
            {"salary", 50000.0},
            {"score", 85.0}
        }
    );

    std::cout << "\n[1] Built document with "
              << doc->getCommon().size() << " common field(s) and "
              << doc->getComparable().size() << " comparable field(s)\n";

    // 2. Chain wiring
    std::unique_ptr<DocCompletionCheckNode> docCheck(
        new DocCompletionCheckNode("DocCompletionCheck"));

    std::unique_ptr<RangeCheckNode> rangeChk(
        new RangeCheckNode("RangeCheck", standardRules()));

    docCheck->setNext(std::move(rangeChk));

    std::cout << "[2] Chain head = " << docCheck->getName()
              << ", next = " << docCheck->getNext()->getName() << "\n";

    // 3. Happy path through the whole chain
    {
        std::cout << "\n[3] Happy path (fully valid document)\n";

        WorkflowInstance wi("WF-001", doc);

        wi.setChain(std::move(docCheck));

        runChainManually(wi.getChain(), wi);
    }

    // 4. DocCompletionCheckNode failure cases
    {
        std::cout << "\n[4a] Empty common field (address = \"\")\n";

        std::shared_ptr<Document> bad = makeDoc(
            {
                {"name", "Jane"},
                {"address", ""},
                {"email", "jane@x.com"}
            },
            {
                {"age", 25.0},
                {"salary", 40000.0},
                {"score", 90.0}
            }
        );

        WorkflowInstance wi("WF-002", bad);

        DocCompletionCheckNode n("DocCompletionCheck");

        std::cout << "    " << n.getName()
                  << " -> " << str(n.handle(wi)) << "\n";

        std::cout << "[4b] NaN comparable field (salary = NaN)\n";

        std::shared_ptr<Document> bad2 = makeDoc(
            {
                {"name", "Jim"},
                {"address", "1 A St"},
                {"email", "jim@x.com"}
            },
            {
                {"age", 25.0},
                {"salary", std::nan("")},
                {"score", 90.0}
            }
        );

        WorkflowInstance wi2("WF-003", bad2);

        DocCompletionCheckNode n2("DocCompletionCheck");

        std::cout << "    " << n2.getName()
                  << " -> " << str(n2.handle(wi2)) << "\n";
    }

    // 5. RangeCheckNode failure cases
    {
        std::cout << "\n[5a] Age below threshold (15 < 18)\n";

        std::shared_ptr<Document> d1 = makeDoc(
            {
                {"name", "Kid"},
                {"address", "2 B St"},
                {"email", "kid@x.com"}
            },
            {
                {"age", 15.0},
                {"salary", 50000.0},
                {"score", 85.0}
            }
        );

        WorkflowInstance wi("WF-004", d1);

        RangeCheckNode n("RangeCheck", standardRules());

        std::cout << "    " << n.getName()
                  << " -> " << str(n.handle(wi)) << "\n";

        std::cout << "[5b] Missing comparable key (no 'salary')\n";

        std::shared_ptr<Document> d2 = makeDoc(
            {
                {"name", "NoPay"}
            },
            {
                {"age", 30.0},
                {"score", 85.0}
            }
        );

        WorkflowInstance wi2("WF-005", d2);

        RangeCheckNode n2("RangeCheck", standardRules());

        std::cout << "    " << n2.getName()
                  << " -> " << str(n2.handle(wi2)) << "\n";
    }

    // 6. WorkflowDefinition
    {
        std::cout << "\n[6] WorkflowDefinition head handling\n";

        WorkflowDefinition def("loan");

        std::unique_ptr<WorkflowNode> head(
            new DocCompletionCheckNode("Head"));

        def.setHead(std::move(head));

        std::cout << "    definition head = "
                  << def.getHead()->getName() << "\n";

        // createChain() is not implemented yet, so skip it.
    }

    // 7. clone()
    {
        std::cout << "\n[7] clone()\n";

        DocCompletionCheckNode dc("OrigDoc");

        std::unique_ptr<WorkflowNode> dc2 = dc.clone();

        std::cout << "    DocCompletionCheck clone -> "
                  << dc2->getName() << "\n";

        RangeCheckNode rc("OrigRange", standardRules());

        std::unique_ptr<WorkflowNode> rc2 = rc.clone();

        std::cout << "    RangeCheck clone         -> "
                  << rc2->getName() << "\n";
    }

    std::cout << "\n===== done =====\n";

    return 0;
}
