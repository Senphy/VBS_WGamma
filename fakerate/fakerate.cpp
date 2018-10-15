//#include "fit.C"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace RooFit;

Int_t    nBins    = 13;
Int_t    binlow   = 2;
Int_t    binhigh  = 5;
Double_t lowp[8]  = {25, 30, 40, 50, 70, 100, 135};
Double_t highp[8] = {30, 40, 50, 70, 100, 135, 400};

void fit(int k);

int main() {
    for (int i = 0; i < 8; i++) {
        fit(i);
    }
    return 0;
}
void fit(int k) {

    TFile* fdata = TFile::Open(Form("wajet_%0.f_%0.f_data.root", lowp[k], highp[k]));
    TFile* ftrue = TFile::Open(Form("wa_%0.f_%0.f_true.root", lowp[k], highp[k]));
    //	TFile* ftrue = TFile::Open(Form("../fit_produce/ZA_%0.f_%0.f_true.root", lowp[k], highp[k]));
    TFile* ffake = TFile::Open(Form("wajet_%0.f_%0.f_fake.root", lowp[k], highp[k]));
    //	TFile* ffake = TFile::Open(Form("../fit_produce/ZJets_FX_%0.f_%0.f_fake.root", lowp[k], highp[k]));

    TH1F* hdata_ = (TH1F*)fdata->Get("histo");
    TH1F* htrue_ = (TH1F*)ftrue->Get("histo");
    TH1F* hfake_ = (TH1F*)ffake->Get("histo");

    Double_t bins[nBins + 1];
    for (Int_t i = 0; i < nBins + 1; i++) {
        bins[i] = 0.0052 + 0.001 * i;
    }
    TH1* hdata = hdata_->Rebin(nBins, "hdata", bins);
    TH1* htrue = htrue_->Rebin(nBins, "htrue", bins);
    TH1* hfake = hfake_->Rebin(nBins, "hfake", bins);
    for (Int_t i = 0; i < nBins + 1; i++) {
        if (hdata->GetBinContent(i + 1) <= 0)
            hdata->SetBinContent(i + 1, 0.000000);
        if (htrue->GetBinContent(i + 1) <= 0)
            htrue->SetBinContent(i + 1, 0.000000);
        if (hfake->GetBinContent(i + 1) <= 0)
            hfake->SetBinContent(i + 1, 0.000000);
    }

    Double_t   nData = hdata->GetSumOfWeights();
    RooRealVar sieie("sieie", "sieie", 0.0052, 0.0182);

    RooDataHist data_hist("datahist", "datahist", sieie, hdata);
    RooDataHist true_hist("truehist", "truehist", sieie, htrue);
    RooDataHist fake_hist("fakehist", "fakehist", sieie, hfake);

    RooHistPdf true_pdf("truepdf", "truepdf", sieie, true_hist);
    RooHistPdf fake_pdf("fakepdf", "fakepdf", sieie, fake_hist);

    RooRealVar nTrue("true number", "true number", 0.5 * nData, 0, nData);
    RooRealVar nFake("fake number", "fake number", 0.5 * nData, 0, nData);

    RooExtendPdf etrue_pdf("ntrue", "ntrue", true_pdf, nTrue);
    RooExtendPdf efake_pdf("nfake", "nfake", fake_pdf, nFake);

    RooAddPdf fullpdf("full_pdf", "true_plus_fake", RooArgList(etrue_pdf, efake_pdf));

    fullpdf.fitTo(data_hist, SumW2Error(kFALSE), Extended(kTRUE));
    RooChi2Var chi2("chi2", "chi2", fullpdf, data_hist);
    Double_t   chi2val   = chi2.getVal();
    Double_t   chi2ToNDF = chi2.getVal() / (nBins - 2);

    RooPlot* xframe = sieie.frame(Title(Form("Barrel region, %0.f GeV < photon PT < %0.f GeV", lowpt[k], highpt[k])), Bins(nBins));
    data_hist.plotOn(xframe);
    fullpdf.plotOn(xframe, Name("sum"), LineColor(kRed));
    fullpdf.plotOn(xframe, Components("ntrue"), Name("true"), LineColor(kGreen), LineStyle(9));
    fullpdf.plotOn(xframe, Components("nfake"), Name("fake"), LineColor(kBlue), LineStyle(9));

    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
    xframe->GetXaxis()->SetTitle("#sigma_{i#etai#eta}");
    xframe->GetYaxis()->SetTitleOffset(1.4);
    xframe->Draw();

    hdata->SetLineColor(kRed);
    hdata->SetLineWidth(3);
    hfake->SetLineColor(kBlue);
    hfake->SetLineStyle(5);
    hfake->SetLineWidth(3);
    htrue->SetLineColor(kGreen);
    htrue->SetLineStyle(5);
    htrue->SetLineWidth(3);
    TLegend* leg = new TLegend(0.7, 0.8, 0.88, 0.88, NULL, "brNDC");
    leg->SetFillColor(10);
    leg->AddEntry(hdata, "Fit result", "L");
    leg->AddEntry(htrue, "Ture photons", "L");
    leg->AddEntry(hfake, "Fake photons", "L");
    leg->Draw("same");

    TString strChi   = "#chi^{2}/ndf=";
    float   chiFloat = (100 * chi2ToNDF);
    int     chiInt   = chiFloat;
    strChi += (chiInt / 100);
    strChi += ".";
    strChi += (chiInt % 100);
    TLatex* textChi2 = new TLatex(0.7, 0.75, strChi);
    textChi2->SetNDC();
    textChi2->SetTextSize(0.03);
    textChi2->SetLineWidth(1);
    textChi2->Draw();

    Double_t nDataInWindowErr = 0.;
    Double_t nDataInWindow_1  = hdata->Integral(1, 1);
    Double_t nDataInWindow_2  = hdata->Integral(2, 2);
    Double_t nDataInWindow_3  = hdata->Integral(3, 3);
    Double_t nDataInWindow_4  = hdata->Integral(4, 4);
    Double_t nDataInWindow_5  = hdata->Integral(5, 5);
    Double_t nDataInWindow_6  = hdata->Integral(6, 6);
    Double_t nDataInWindow_7  = hdata->Integral(7, 7);
    Double_t nDataInWindow    = hdata->IntegralAndError(binlow, binhigh, nDataInWindowErr);
    cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
    cout << "hdata(1)_lowedge:" << hdata->GetBinLowEdge(1) << "\t"
         << "hdata(1)_content:" << hdata->GetBinContent(1) << endl;
    Double_t nDatatotal   = hdata->Integral();
    Double_t nTrue_fit    = nTrue.getVal();
    Double_t nTrue_fitErr = nTrue.getError();
    Double_t nFake_fit    = nFake.getVal();
    Double_t nFake_fitErr = nFake.getError();

    sieie.setRange("window", 0.0062, 0.0102);

    RooRealVar* fracFake          = (RooRealVar*)efake_pdf.createIntegral(sieie, sieie, "window");
    Double_t    nFake_inwindow    = nFake_fit * fracFake->getVal();
    Double_t    nFake_inwindowErr = sqrt(nFake_fitErr * nFake_fitErr * fracFake->getVal() * fracFake->getVal() + nFake_fit * nFake_fit * fracFake->getError() * fracFake->getError());
    RooRealVar* fracTrue          = (RooRealVar*)etrue_pdf.createIntegral(sieie, sieie, "window");
    Double_t    nTrue_inwindow    = nTrue_fit * fracTrue->getVal();
    Double_t    nTrue_inwindowErr = sqrt(nTrue_fitErr * nTrue_fitErr * fracTrue->getVal() * fracTrue->getVal() + nTrue_fit * nTrue_fit * fracTrue->getError() * fracTrue->getError());

    Double_t fakerate    = nFake_inwindow / nDataInWindow;
    Double_t fakerateErr = sqrt(nFake_inwindowErr * nFake_inwindowErr / (nDataInWindow * nDataInWindow)
                                + nFake_inwindow * nFake_inwindow * nDataInWindowErr * nDataInWindowErr / (nDataInWindow * nDataInWindow * nDataInWindow * nDataInWindow));

    ofstream myfile(TString("fakerate_") + Form("photon_pt%0.f_%0.f.txt", lowpt[k], highpt[k]), ios::out);

    myfile << "data in window = " << nDataInWindow << "+-" << nDataInWindowErr << " " << nDataInWindow_1 << " " << nDataInWindow_2 << " " << nDataInWindow_3 << " " << nDataInWindow_4 << " " << nDataInWindow_5 << " " << nDataInWindow_6 << " " << nDataInWindow_7 << std::endl;
    myfile << "nDatatotal = " << nDatatotal << std::endl;
    myfile << "fake number = " << nFake_fit << "+-" << nFake_fitErr << std::endl;
    myfile << "true number = " << nTrue_fit << "+-" << nTrue_fitErr << std::endl;
    myfile << "fake in window = " << nFake_inwindow << "+-" << nFake_inwindowErr << std::endl;
    myfile << "true in window = " << nTrue_inwindow << "+-" << nTrue_inwindowErr << std::endl;
    myfile << "fakerate = " << fakerate << "+-" << fakerateErr << std::endl;
    myfile << "chi2ToNDF = " << chi2ToNDF << std::endl;

    TString strFR   = "FR = (";
    float   FRFloat = (1000 * fakerate);
    int     FRInt   = FRFloat;
    strFR += (FRInt / 10);
    strFR += ".";
    strFR += (FRInt % 10);
    strFR += "+-";
    float FRErrFloat = (1000 * fakerateErr);
    int   FRErrInt   = FRErrFloat;
    strFR += (FRErrInt / 10);
    strFR += ".";
    strFR += (FRErrInt % 10);
    strFR += ")";
    TLatex* textFR = new TLatex(0.67, 0.7, strFR);
    textFR->SetNDC();
    textFR->SetTextSize(0.03);
    textFR->SetLineWidth(1);
    textFR->Draw();

    char buffer[256];
    sprintf(buffer, "pt_%0.f_%0.f.png", lowpt[k], highpt[k]);
    c1->SaveAs(buffer);
}
