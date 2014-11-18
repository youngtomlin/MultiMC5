#include "QuickModRepoDialog.h"
#include "ui_QuickModRepoDialog.h"

#include "logic/quickmod/QuickModDatabase.h"
#include "logic/quickmod/QuickModIndexModel.h"
#include "MultiMC.h"

QuickModRepoDialog::QuickModRepoDialog(QWidget *parent)
	: QDialog(parent), ui(new Ui::QuickModRepoDialog)
{
	ui->setupUi(this);

	ui->treeView->setModel(new QuickModIndexModel);
	ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->treeView->setSelectionMode(QTreeView::ExtendedSelection);
	ui->treeView->setSelectionBehavior(QTreeView::SelectRows);
}

QuickModRepoDialog::~QuickModRepoDialog()
{
	delete ui;
}

void QuickModRepoDialog::on_removeBtn_clicked()
{
	QModelIndexList indexes = ui->treeView->selectionModel()->selectedRows();
	for (const auto index : indexes)
	{
		const auto repo = index.data(Qt::UserRole).toString();
		MMC->qmdb()->removeRepo(repo);
	}
}
