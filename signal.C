#include <iostream>
#include <TFile.h>
#include <TTree.h>

//The data we had was in CERN ROOT file in tree format
//This code has been used to convert the data in the ROOT file where there were many trees but we wanted a particular tree for our analysis
//We tried to make the new tree in a new root file with only desired branches but failed to do so with this code 
//This code gave us the desired tree in a new file but all the branches were copied
//For that we had to use extra space creating a new CSV file with the partiuclar branches which will become the features ultimately


void copy_tree(const std::string& input_file_name,
               const std::string& output_file_name,
               const std::string& tree_name,
               const std::vector<std::string>& branch_names)
{
    // Open input file
    TFile* input_file = TFile::Open(input_file_name.c_str(), "READ");
    if (!input_file || input_file->IsZombie()) {
        std::cerr << "Error: could not open input file " << input_file_name << std::endl;
        return;
    }

    // the input tree
    TTree* input_tree = dynamic_cast<TTree*>(input_file->Get(tree_name.c_str()));
    if (!input_tree) {
        std::cerr << "Error: could not find tree " << tree_name << " in file " << input_file_name << std::endl;
        input_file->Close();
        return;
    }

    // Create output file
    TFile* output_file = TFile::Open(output_file_name.c_str(), "RECREATE");
    if (!output_file || output_file->IsZombie()) {
        std::cerr << "Error: could not create output file " << output_file_name << std::endl;
        input_file->Close();
        return;
    }

    // Create a new tree in the output file
    TTree* output_tree = input_tree->CloneTree(0);

    // Set the branch addresses for the input tree
    std::vector<Float_t*> branch_addrs(branch_names.size());
    for (size_t i = 0; i < branch_names.size(); ++i) {
        TBranch* branch = input_tree->FindBranch(branch_names[i].c_str());
        if (!branch) {
            std::cerr << "Warning: could not find branch " << branch_names[i] << " in tree " << tree_name << std::endl;
            continue;
        }
        Float_t* addr = new Float_t;
        branch->SetAddress(addr);
        branch_addrs[i] = addr;
    }

    //fill the output tree with the selected branches
    Long64_t num_entries = input_tree->GetEntries();
    for (Long64_t i = 0; i < num_entries; ++i) {
        input_tree->GetEntry(i);
        output_tree->Fill();
    }

    // Write the output file and close both files
    output_file->Write();
    input_file->Close();
    output_file->Close();

    // Delete the branch addresses
    for (size_t i = 0; i < branch_addrs.size(); ++i) {
        delete branch_addrs[i];
    }
}

void Signal()
{
    // Set the input file name, output file name, tree name, and branch names
    std::string input_file_name = "output_VBFHToGG_M125_13TeV_amcatnlo_pythia8.root";
    std::string output_file_name = "signal_file.root";
    std::string tree_name = "/tagsDumper/trees/vbf_125_13TeV_THQLeptonicTag";
    std::vector<std::string> branch_names = {"ele1_pt", "ele2_pt"};

    // Copy the selected branches from the input tree to the output file
    copy_tree(input_file_name, output_file_name, tree_name, branch_names);
}

